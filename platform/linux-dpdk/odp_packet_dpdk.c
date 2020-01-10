/* Copyright (c) 2013-2018, Linaro Limited
 * Copyright (c) 2019, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include <odp_posix_extensions.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include <linux/ethtool.h>
#include <linux/sockios.h>

#include <odp/api/cpu.h>
#include <odp/api/hints.h>
#include <odp/api/system_info.h>
#include <odp_debug_internal.h>
#include <odp_errno_define.h>
#include <odp_classification_internal.h>
#include <odp_packet_io_internal.h>
#include <odp_libconfig_internal.h>
#include <odp/api/plat/packet_inlines.h>
#include <odp/api/time.h>
#include <odp/api/plat/time_inlines.h>
#include <odp_packet_dpdk.h>
#include <odp_eventdev_internal.h>
#include <protocols/eth.h>

#include <net/if.h>
#include <protocols/udp.h>

#include <rte_config.h>
#if defined(__clang__)
#undef RTE_TOOLCHAIN_GCC
#endif
#include <rte_bus_vdev.h>
#include <rte_ethdev.h>
#include <rte_eth_ring.h>
#include <rte_ip_frag.h>
#include <rte_udp.h>
#include <rte_tcp.h>
#include <rte_version.h>

#if RTE_VERSION < RTE_VERSION_NUM(19, 8, 0, 0)
#define rte_ether_addr ether_addr
#define rte_ipv4_hdr ipv4_hdr
#define rte_ipv6_hdr ipv6_hdr
#define rte_tcp_hdr tcp_hdr
#define rte_udp_hdr udp_hdr
#endif

/* DPDK poll mode drivers requiring minimum RX burst size DPDK_MIN_RX_BURST */
#define IXGBE_DRV_NAME "net_ixgbe"
#define I40E_DRV_NAME "net_i40e"

/* Minimum RX burst size */
#define DPDK_MIN_RX_BURST 4

/* Loopback interface ring size */
#define LOOPBACK_RING_SIZE 512

/* Number of packet buffers to prefetch in RX */
#define NUM_RX_PREFETCH 4

/** DPDK runtime configuration options */
typedef struct {
	int num_rx_desc;
	int num_tx_desc;
	int rx_drop_en;
} dpdk_opt_t;

struct pkt_cache_t {
	/* Array for storing extra RX packets */
	struct rte_mbuf *pkt[DPDK_MIN_RX_BURST];
	unsigned int idx;   /* Head of cache */
	unsigned int count; /* Number of packets in cache */
};

typedef union ODP_ALIGNED_CACHE {
	struct pkt_cache_t s;
	uint8_t pad[ROUNDUP_CACHE_LINE(sizeof(struct pkt_cache_t))];
} pkt_cache_t;

/** Packet socket using dpdk mmaped rings for both Rx and Tx */
typedef struct ODP_ALIGNED_CACHE {
	uint16_t port_id;		  /**< DPDK port identifier */
	uint16_t mtu;			  /**< maximum transmission unit */
	uint8_t lockless_rx;		  /**< no locking for rx */
	uint8_t lockless_tx;		  /**< no locking for tx */
	uint8_t min_rx_burst;		  /**< minimum RX burst size */
	uint8_t loopback;		  /**< Operate as loopback interface */
	odp_pktin_hash_proto_t hash;	  /**< Packet input hash protocol */
	/* Supported RTE_PTYPE_XXX flags in a mask */
	uint32_t supported_ptypes;
	char ifname[32];
	/* Cache for storing extra RX packets */
	pkt_cache_t rx_cache[PKTIO_MAX_QUEUES];
	/** RX queue locks */
	odp_ticketlock_t ODP_ALIGNED_CACHE rx_lock[PKTIO_MAX_QUEUES];
	odp_ticketlock_t tx_lock[PKTIO_MAX_QUEUES];  /**< TX queue locks */
	uint8_t vdev_sysc_promisc;	/**< promiscuous mode defined with
					    system call */
	dpdk_opt_t opt;
	/* RX callback functions */
#if RTE_VERSION < RTE_VERSION_NUM(18, 05, 0, 0)
	struct rte_eth_rxtx_callback *rx_callback[PKTIO_MAX_QUEUES];
#else
	const struct rte_eth_rxtx_callback *rx_callback[PKTIO_MAX_QUEUES];
#endif
} pkt_dpdk_t;

ODP_STATIC_ASSERT(PKTIO_PRIVATE_SIZE >= sizeof(pkt_dpdk_t),
		  "PKTIO_PRIVATE_SIZE too small");

/* Global DPDK pktio data */
typedef struct ODP_ALIGNED_CACHE {
	odp_shm_t shm;			/* SHM handle for global data */
	struct rte_ring *loopback_ring;	/* Loopback interface data ring */
	uint16_t loopback_port_id;	/* Loopback interface port id */
	uint8_t loopback_in_use;	/* Loopback interface in use */
} dpdk_global_t;

static dpdk_global_t *dpdk_glb;

static inline pkt_dpdk_t *pkt_priv(pktio_entry_t *pktio_entry)
{
	return (pkt_dpdk_t *)(uintptr_t)(pktio_entry->s.pkt_priv);
}

/* Ops for all implementation of pktio.
 * Order matters. The first implementation to setup successfully
 * will be picked.
 * Array must be NULL terminated */
const pktio_if_ops_t * const pktio_if_ops[]  = {
	&dpdk_pktio_ops,
	&null_pktio_ops,
	NULL
};

extern void *pktio_entry_ptr[ODP_CONFIG_PKTIO_ENTRIES];

static uint32_t mtu_get_pkt_dpdk(pktio_entry_t *pktio_entry);

uint16_t dpdk_pktio_port_id(pktio_entry_t *pktio_entry)
{
	const pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);

	return pkt_dpdk->port_id;
}

static int lookup_opt(const char *opt_name, const char *drv_name, int *val)
{
	const char *base = "pktio_dpdk";
	int ret;

	ret = _odp_libconfig_lookup_ext_int(base, drv_name, opt_name, val);
	if (ret == 0)
		ODP_ERR("Unable to find DPDK configuration option: %s\n",
			opt_name);

	return ret;
}

static int init_options(pktio_entry_t *pktio_entry,
			const struct rte_eth_dev_info *dev_info)
{
	dpdk_opt_t *opt = &pkt_priv(pktio_entry)->opt;

	if (!lookup_opt("num_rx_desc", dev_info->driver_name,
			&opt->num_rx_desc))
		return -1;
	if (opt->num_rx_desc < dev_info->rx_desc_lim.nb_min ||
	    opt->num_rx_desc > dev_info->rx_desc_lim.nb_max ||
	    opt->num_rx_desc % dev_info->rx_desc_lim.nb_align) {
		ODP_ERR("Invalid number of RX descriptors\n");
		return -1;
	}

	if (!lookup_opt("num_tx_desc", dev_info->driver_name,
			&opt->num_tx_desc))
		return -1;
	if (opt->num_tx_desc < dev_info->tx_desc_lim.nb_min ||
	    opt->num_tx_desc > dev_info->tx_desc_lim.nb_max ||
	    opt->num_tx_desc % dev_info->tx_desc_lim.nb_align) {
		ODP_ERR("Invalid number of TX descriptors\n");
		return -1;
	}

	if (!lookup_opt("rx_drop_en", dev_info->driver_name,
			&opt->rx_drop_en))
		return -1;
	opt->rx_drop_en = !!opt->rx_drop_en;

	ODP_PRINT("DPDK interface (%s): %" PRIu16 "\n", dev_info->driver_name,
		  pkt_priv(pktio_entry)->port_id);
	ODP_PRINT("  num_rx_desc: %d\n", opt->num_rx_desc);
	ODP_PRINT("  num_tx_desc: %d\n", opt->num_tx_desc);
	ODP_PRINT("  rx_drop_en: %d\n", opt->rx_drop_en);

	return 0;
}

/* Test if s has only digits or not. Dpdk pktio uses only digits.*/
static int _dpdk_netdev_is_valid(const char *s)
{
	while (*s) {
		if (!isdigit(*s))
			return 0;
		s++;
	}

	return 1;
}

static void rss_conf_to_hash_proto(struct rte_eth_rss_conf *rss_conf,
				   const odp_pktin_hash_proto_t *hash_proto)
{
	if (hash_proto->proto.ipv4_udp)
		rss_conf->rss_hf |= ETH_RSS_NONFRAG_IPV4_UDP;
	if (hash_proto->proto.ipv4_tcp)
		rss_conf->rss_hf |= ETH_RSS_NONFRAG_IPV4_TCP;
	if (hash_proto->proto.ipv4)
		rss_conf->rss_hf |= ETH_RSS_IPV4 | ETH_RSS_FRAG_IPV4 |
				    ETH_RSS_NONFRAG_IPV4_OTHER;
	if (hash_proto->proto.ipv6_udp)
		rss_conf->rss_hf |= ETH_RSS_NONFRAG_IPV6_UDP |
				    ETH_RSS_IPV6_UDP_EX;
	if (hash_proto->proto.ipv6_tcp)
		rss_conf->rss_hf |= ETH_RSS_NONFRAG_IPV6_TCP |
				    ETH_RSS_IPV6_TCP_EX;
	if (hash_proto->proto.ipv6)
		rss_conf->rss_hf |= ETH_RSS_IPV6 | ETH_RSS_FRAG_IPV6 |
				    ETH_RSS_NONFRAG_IPV6_OTHER |
				    ETH_RSS_IPV6_EX;
	rss_conf->rss_key = NULL;
}

static int dpdk_setup_eth_dev(pktio_entry_t *pktio_entry,
			      const struct rte_eth_dev_info *dev_info)
{
	int ret;
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	struct rte_eth_rss_conf rss_conf;
	struct rte_eth_conf eth_conf;
	uint64_t rss_hf_capa = dev_info->flow_type_rss_offloads;
	pool_t *pool = pool_entry_from_hdl(pktio_entry->s.pool);
	uint64_t rx_offloads = 0;
	uint64_t tx_offloads = 0;

	memset(&rss_conf, 0, sizeof(struct rte_eth_rss_conf));

	/* Always set some hash functions to enable DPDK RSS hash calculation.
	 * Hash capability has been checked in pktin config. */
	if (pkt_dpdk->hash.all_bits == 0)
		rss_conf.rss_hf = ETH_RSS_IP | ETH_RSS_TCP | ETH_RSS_UDP;
	else
		rss_conf_to_hash_proto(&rss_conf, &pkt_dpdk->hash);

	/* Filter out unsupported flags */
	rss_conf.rss_hf &= rss_hf_capa;

	memset(&eth_conf, 0, sizeof(eth_conf));

	eth_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
	eth_conf.txmode.mq_mode = ETH_MQ_TX_NONE;
	eth_conf.rx_adv_conf.rss_conf = rss_conf;

	/* Setup RX checksum offloads */
	if (pktio_entry->s.config.pktin.bit.ipv4_chksum)
		rx_offloads |= DEV_RX_OFFLOAD_IPV4_CKSUM;

	if (pktio_entry->s.config.pktin.bit.udp_chksum)
		rx_offloads |= DEV_RX_OFFLOAD_UDP_CKSUM;

	if (pktio_entry->s.config.pktin.bit.tcp_chksum)
		rx_offloads |= DEV_RX_OFFLOAD_TCP_CKSUM;

	eth_conf.rxmode.offloads = rx_offloads;

	/* Setup TX checksum offloads */
	if (pktio_entry->s.config.pktout.bit.ipv4_chksum_ena)
		tx_offloads |= DEV_TX_OFFLOAD_IPV4_CKSUM;

	if (pktio_entry->s.config.pktout.bit.udp_chksum_ena)
		tx_offloads |= DEV_TX_OFFLOAD_UDP_CKSUM;

	if (pktio_entry->s.config.pktout.bit.tcp_chksum_ena)
		tx_offloads |= DEV_TX_OFFLOAD_TCP_CKSUM;

	if (pktio_entry->s.config.pktout.bit.sctp_chksum_ena)
		tx_offloads |= DEV_TX_OFFLOAD_SCTP_CKSUM;

	eth_conf.txmode.offloads = tx_offloads;

	if (tx_offloads)
		pktio_entry->s.chksum_insert_ena = 1;

	/* RX packet len same size as pool segment minus headroom and double
	 * VLAN tag
	 */
	eth_conf.rxmode.max_rx_pkt_len =
		rte_pktmbuf_data_room_size(pool->rte_mempool) -
		2 * 4 - RTE_PKTMBUF_HEADROOM;

	ret = rte_eth_dev_configure(pkt_dpdk->port_id,
				    pktio_entry->s.num_in_queue,
				    pktio_entry->s.num_out_queue, &eth_conf);
	if (ret < 0) {
		ODP_ERR("Failed to setup device: err=%d, port=%" PRIu8 "\n",
			ret, pkt_dpdk->port_id);
		return -1;
	}
	return 0;
}

static void _dpdk_print_port_mac(uint16_t port_id)
{
	struct rte_ether_addr eth_addr;

	memset(&eth_addr, 0, sizeof(eth_addr));
	rte_eth_macaddr_get(port_id, &eth_addr);
	ODP_DBG("Port %" PRIu16 ", "
		"MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
		port_id,
		eth_addr.addr_bytes[0],
		eth_addr.addr_bytes[1],
		eth_addr.addr_bytes[2],
		eth_addr.addr_bytes[3],
		eth_addr.addr_bytes[4],
		eth_addr.addr_bytes[5]);
}

static int check_hash_proto(pktio_entry_t *pktio_entry,
			    const odp_pktin_queue_param_t *p)
{
	struct rte_eth_dev_info dev_info;
	uint64_t rss_hf_capa;
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	uint16_t port_id = pkt_dpdk->port_id;

	rte_eth_dev_info_get(port_id, &dev_info);
	rss_hf_capa = dev_info.flow_type_rss_offloads;

	if (p->hash_proto.proto.ipv4 &&
	    ((rss_hf_capa & ETH_RSS_IPV4) == 0)) {
		ODP_ERR("hash_proto.ipv4 not supported\n");
		return -1;
	}

	if (p->hash_proto.proto.ipv4_udp &&
	    ((rss_hf_capa & ETH_RSS_NONFRAG_IPV4_UDP) == 0)) {
		ODP_ERR("hash_proto.ipv4_udp not supported. "
			"rss_hf_capa 0x%" PRIx64 "\n", rss_hf_capa);
		return -1;
	}

	if (p->hash_proto.proto.ipv4_tcp &&
	    ((rss_hf_capa & ETH_RSS_NONFRAG_IPV4_TCP) == 0)) {
		ODP_ERR("hash_proto.ipv4_tcp not supported. "
			"rss_hf_capa 0x%" PRIx64 "\n", rss_hf_capa);
		return -1;
	}

	if (p->hash_proto.proto.ipv6 &&
	    ((rss_hf_capa & ETH_RSS_IPV6) == 0)) {
		ODP_ERR("hash_proto.ipv6 not supported. "
			"rss_hf_capa 0x%" PRIx64 "\n", rss_hf_capa);
		return -1;
	}

	if (p->hash_proto.proto.ipv6_udp &&
	    ((rss_hf_capa & ETH_RSS_NONFRAG_IPV6_UDP) == 0)) {
		ODP_ERR("hash_proto.ipv6_udp not supported. "
			"rss_hf_capa 0x%" PRIx64 "\n", rss_hf_capa);
		return -1;
	}

	if (p->hash_proto.proto.ipv6_tcp &&
	    ((rss_hf_capa & ETH_RSS_NONFRAG_IPV6_TCP) == 0)) {
		ODP_ERR("hash_proto.ipv6_tcp not supported. "
			"rss_hf_capa 0x%" PRIx64 "\n", rss_hf_capa);
		return -1;
	}

	return 0;
}

static int dpdk_input_queues_config(pktio_entry_t *pktio_entry,
				    const odp_pktin_queue_param_t *p)
{
	odp_pktin_mode_t mode = pktio_entry->s.param.in_mode;
	uint8_t lockless;

	if (p->hash_enable && p->num_queues > 1 &&
	    check_hash_proto(pktio_entry, p))
		return -1;

	/**
	 * Scheduler synchronizes input queue polls. Only single thread
	 * at a time polls a queue */
	if (mode == ODP_PKTIN_MODE_SCHED ||
	    p->op_mode == ODP_PKTIO_OP_MT_UNSAFE)
		lockless = 1;
	else
		lockless = 0;

	if (p->hash_enable && p->num_queues > 1)
		pkt_priv(pktio_entry)->hash = p->hash_proto;

	pkt_priv(pktio_entry)->lockless_rx = lockless;

	return 0;
}

static int dpdk_output_queues_config(pktio_entry_t *pktio_entry,
				     const odp_pktout_queue_param_t *p)
{
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	uint8_t lockless;

	if (p->op_mode == ODP_PKTIO_OP_MT_UNSAFE)
		lockless = 1;
	else
		lockless = 0;

	pkt_dpdk->lockless_tx = lockless;

	return 0;
}

static int dpdk_init_global(void)
{
	odp_shm_t shm;

	shm = odp_shm_reserve("_odp_dpdk_gbl", sizeof(dpdk_global_t),
			      sizeof(dpdk_global_t), 0);
	if (shm == ODP_SHM_INVALID)
		return -1;

	dpdk_glb = odp_shm_addr(shm);

	memset(dpdk_glb, 0, sizeof(dpdk_global_t));
	dpdk_glb->shm = shm;
	dpdk_glb->loopback_port_id = -1;

	return 0;
}

static int dpdk_term_global(void)
{
	int ret = 0;

	/* Eventdev takes care of closing pktio devices */
	if (!eventdev_gbl ||
	    eventdev_gbl->rx_adapter.status == RX_ADAPTER_INIT) {
		uint16_t port_id;

		if (dpdk_glb->loopback_ring) {
			char name[RTE_ETH_NAME_MAX_LEN];

			port_id = dpdk_glb->loopback_port_id;
			if (rte_eth_dev_get_name_by_port(port_id, name)) {
				ODP_ERR("Reading loopback interface name failed\n");
				ret = -1;
			} else {
				if (rte_vdev_uninit(name)) {
					ODP_ERR("Uninitializing loopback interface failed\n");
					ret = -1;
				}
				rte_ring_free(dpdk_glb->loopback_ring);
			}
		}

		RTE_ETH_FOREACH_DEV(port_id) {
			rte_eth_dev_close(port_id);
		}
	}

	if (odp_shm_free(dpdk_glb->shm)) {
		ODP_ERR("shm free failed");
		return -1;
	}

	return ret;
}

static int dpdk_init_capability(pktio_entry_t *pktio_entry,
				struct rte_eth_dev_info *dev_info)
{
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	odp_pktio_capability_t *capa = &pktio_entry->s.capa;
	struct rte_ether_addr mac_addr;
	int ret;
	int ptype_cnt;
	int ptype_l3_ipv4 = 0;
	int ptype_l4_tcp = 0;
	int ptype_l4_udp = 0;
	uint32_t ptype_mask = RTE_PTYPE_L3_MASK | RTE_PTYPE_L4_MASK;

	memset(dev_info, 0, sizeof(struct rte_eth_dev_info));
	memset(capa, 0, sizeof(odp_pktio_capability_t));

	rte_eth_dev_info_get(pkt_dpdk->port_id, dev_info);
	capa->max_input_queues = RTE_MIN(dev_info->max_rx_queues,
					 PKTIO_MAX_QUEUES);

	/* ixgbe devices support only 16 RX queues in RSS mode */
	if (!strncmp(dev_info->driver_name, IXGBE_DRV_NAME,
		     strlen(IXGBE_DRV_NAME)))
		capa->max_input_queues = RTE_MIN(16,
						 (int)capa->max_input_queues);

	capa->max_output_queues = RTE_MIN(dev_info->max_tx_queues,
					  PKTIO_MAX_QUEUES);
	capa->set_op.op.promisc_mode = 1;

	/* Ring pmd doesn't support setting MAC or enabling promisc mode */
	if (pkt_dpdk->loopback) {
		capa->set_op.op.mac_addr = 0;
		capa->set_op.op.promisc_mode = 0;
	} else {
		/* Check if setting default MAC address is supporter */
		rte_eth_macaddr_get(pkt_dpdk->port_id, &mac_addr);
		ret = rte_eth_dev_default_mac_addr_set(pkt_dpdk->port_id,
						       &mac_addr);
		if (ret == 0) {
			capa->set_op.op.mac_addr = 1;
		} else if (ret != -ENOTSUP && ret != -EPERM) {
			ODP_ERR("Failed to set interface default MAC: %d\n",
				ret);
			return -1;
		}
	}

	ptype_cnt = rte_eth_dev_get_supported_ptypes(pkt_dpdk->port_id,
						     ptype_mask, NULL, 0);
	if (ptype_cnt > 0) {
		uint32_t ptypes[ptype_cnt];
		int i;

		ptype_cnt = rte_eth_dev_get_supported_ptypes(pkt_dpdk->port_id,
							     ptype_mask, ptypes,
							     ptype_cnt);
		for (i = 0; i < ptype_cnt; i++)
			switch (ptypes[i]) {
			case RTE_PTYPE_L3_IPV4:
			/* Fall through */
			case RTE_PTYPE_L3_IPV4_EXT_UNKNOWN:
			/* Fall through */
			case RTE_PTYPE_L3_IPV4_EXT:
				ptype_l3_ipv4 = 1;
				break;
			case RTE_PTYPE_L4_TCP:
				ptype_l4_tcp = 1;
				break;
			case RTE_PTYPE_L4_UDP:
				ptype_l4_udp = 1;
				break;
			}
	}

	odp_pktio_config_init(&capa->config);
	capa->config.pktin.bit.ts_all = 1;
	capa->config.pktin.bit.ts_ptp = 1;

	capa->config.pktin.bit.ipv4_chksum = ptype_l3_ipv4 &&
		(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_IPV4_CKSUM) ? 1 : 0;
	if (capa->config.pktin.bit.ipv4_chksum)
		capa->config.pktin.bit.drop_ipv4_err = 1;

	capa->config.pktin.bit.udp_chksum = ptype_l4_udp &&
		(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_UDP_CKSUM) ? 1 : 0;
	if (capa->config.pktin.bit.udp_chksum)
		capa->config.pktin.bit.drop_udp_err = 1;

	capa->config.pktin.bit.tcp_chksum = ptype_l4_tcp &&
		(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_TCP_CKSUM) ? 1 : 0;
	if (capa->config.pktin.bit.tcp_chksum)
		capa->config.pktin.bit.drop_tcp_err = 1;

	capa->config.pktout.bit.ipv4_chksum =
		(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_IPV4_CKSUM) ? 1 : 0;
	capa->config.pktout.bit.udp_chksum =
		(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_UDP_CKSUM) ? 1 : 0;
	capa->config.pktout.bit.tcp_chksum =
		(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_TCP_CKSUM) ? 1 : 0;

	capa->config.pktout.bit.ipv4_chksum_ena =
		capa->config.pktout.bit.ipv4_chksum;
	capa->config.pktout.bit.udp_chksum_ena =
		capa->config.pktout.bit.udp_chksum;
	capa->config.pktout.bit.tcp_chksum_ena =
		capa->config.pktout.bit.tcp_chksum;

	return 0;
}

static int dpdk_init_loopback(pkt_dpdk_t * const pkt_dpdk)
{
	struct rte_ring *ring;
	int port_id;

	if (dpdk_glb->loopback_in_use) {
		ODP_ERR("Loopback interface reserved\n");
		return -1;
	}

	if (!dpdk_glb->loopback_ring) {
		ring = rte_ring_create("_odp_loopback_ring", LOOPBACK_RING_SIZE,
				       rte_socket_id(), 0);
		if (ring == NULL) {
			ODP_ERR("Creating loopback ring failed\n");
			return -1;
		}

		port_id = rte_eth_from_ring(ring);
		if (port_id == -1) {
			ODP_ERR("Creating loopback interface failed\n");
			rte_ring_free(ring);
			return -1;
		}

		dpdk_glb->loopback_ring = ring;
		dpdk_glb->loopback_port_id = port_id;
		pkt_dpdk->port_id = port_id;
	} else {
		pkt_dpdk->port_id = dpdk_glb->loopback_port_id;
	}

	pkt_dpdk->loopback = 1;

	return 0;
}

/* Some DPDK PMD virtual devices, like PCAP, do not support promisc
 * mode change. Use system call for them. */
static void promisc_mode_check(pkt_dpdk_t *pkt_dpdk)
{
#if RTE_VERSION < RTE_VERSION_NUM(19, 11, 0, 0)
	/* Enable and disable calls do not have return value */
	rte_eth_promiscuous_enable(pkt_dpdk->port_id);

	if (!rte_eth_promiscuous_get(pkt_dpdk->port_id))
		pkt_dpdk->vdev_sysc_promisc = 1;

	rte_eth_promiscuous_disable(pkt_dpdk->port_id);
#else
	int ret;

	ret = rte_eth_promiscuous_enable(pkt_dpdk->port_id);

	if (!rte_eth_promiscuous_get(pkt_dpdk->port_id))
		pkt_dpdk->vdev_sysc_promisc = 1;

	ret += rte_eth_promiscuous_disable(pkt_dpdk->port_id);

	if (ret)
		pkt_dpdk->vdev_sysc_promisc = 1;
#endif
}

static int setup_pkt_dpdk(odp_pktio_t pktio ODP_UNUSED,
			  pktio_entry_t *pktio_entry,
			  const char *netdev, odp_pool_t pool ODP_UNUSED)
{
	uint32_t mtu;
	struct rte_eth_dev_info dev_info;
	pkt_dpdk_t * const pkt_dpdk = pkt_priv(pktio_entry);
	int i;

	if (!strcmp(netdev, "loop")) {
		if (dpdk_init_loopback(pkt_dpdk))
			return -1;
	} else if (!_dpdk_netdev_is_valid(netdev)) {
		ODP_DBG("Interface name should only contain numbers!: %s\n",
			netdev);
		return -1;
	} else {
		pkt_dpdk->port_id = atoi(netdev);

	}

	if (dpdk_init_capability(pktio_entry, &dev_info)) {
		ODP_DBG("Failed to initialize capabilities for interface: %s\n",
			netdev);
		return -1;
	}

	/* Initialize runtime options */
	if (init_options(pktio_entry, &dev_info)) {
		ODP_ERR("Initializing runtime options failed\n");
		return -1;
	}

	/* Drivers requiring minimum burst size. Supports also *_vf versions
	 * of the drivers. */
	if (!strncmp(dev_info.driver_name, IXGBE_DRV_NAME,
		     strlen(IXGBE_DRV_NAME)) ||
	    !strncmp(dev_info.driver_name, I40E_DRV_NAME,
		     strlen(I40E_DRV_NAME)))
		pkt_dpdk->min_rx_burst = DPDK_MIN_RX_BURST;
	else
		pkt_dpdk->min_rx_burst = 0;

	_dpdk_print_port_mac(pkt_dpdk->port_id);

	mtu = mtu_get_pkt_dpdk(pktio_entry);
	if (mtu == 0) {
		ODP_ERR("Failed to read interface MTU\n");
		return -1;
	}
	pkt_dpdk->mtu = mtu + _ODP_ETHHDR_LEN;

	for (i = 0; i < PKTIO_MAX_QUEUES; i++) {
		odp_ticketlock_init(&pkt_dpdk->rx_lock[i]);
		odp_ticketlock_init(&pkt_dpdk->tx_lock[i]);
	}

	if (pkt_dpdk->loopback)
		dpdk_glb->loopback_in_use = 1;

	return 0;
}

static int close_pkt_dpdk(pktio_entry_t *pktio_entry)
{
	pkt_dpdk_t * const pkt_dpdk = pkt_priv(pktio_entry);
	unsigned int i, j;

	if (eventdev_gbl &&
	    eventdev_gbl->rx_adapter.status != RX_ADAPTER_INIT)
		rx_adapter_port_stop(pkt_dpdk->port_id);
	else
		rte_eth_dev_stop(pkt_dpdk->port_id);

	if (pkt_dpdk->loopback)
		dpdk_glb->loopback_in_use = 0;

	/* Free cached packets */
	for (i = 0; i < PKTIO_MAX_QUEUES; i++) {
		unsigned int idx = pkt_dpdk->rx_cache[i].s.idx;

		for (j = 0; j < pkt_dpdk->rx_cache[i].s.count; j++)
			rte_pktmbuf_free(pkt_dpdk->rx_cache[i].s.pkt[idx++]);
	}

	return 0;
}

static int dpdk_setup_eth_tx(pktio_entry_t *pktio_entry,
			     const pkt_dpdk_t *pkt_dpdk,
			     const struct rte_eth_dev_info *dev_info)
{
	uint32_t i;
	int ret;
	uint16_t port_id = pkt_dpdk->port_id;

	for (i = 0; i < pktio_entry->s.num_out_queue; i++) {
		ret = rte_eth_tx_queue_setup(port_id, i,
					     pkt_dpdk->opt.num_tx_desc,
					     rte_eth_dev_socket_id(port_id),
					     &dev_info->default_txconf);
		if (ret < 0) {
			ODP_ERR("Queue setup failed: err=%d, port=%" PRIu8 "\n",
				ret, port_id);
			return -1;
		}
	}

	return 0;
}

static int dpdk_setup_eth_rx(const pktio_entry_t *pktio_entry,
			     const pkt_dpdk_t *pkt_dpdk,
			     const struct rte_eth_dev_info *dev_info)
{
	struct rte_eth_rxconf rxconf;
	uint32_t i;
	int ret;
	uint16_t port_id = pkt_dpdk->port_id;
	pool_t *pool = pool_entry_from_hdl(pktio_entry->s.pool);

	rxconf = dev_info->default_rxconf;

	rxconf.rx_drop_en = pkt_dpdk->opt.rx_drop_en;

	for (i = 0; i < pktio_entry->s.num_in_queue; i++) {
		ret = rte_eth_rx_queue_setup(port_id, i,
					     pkt_dpdk->opt.num_rx_desc,
					     rte_eth_dev_socket_id(port_id),
					     &rxconf, pool->rte_mempool);
		if (ret < 0) {
			ODP_ERR("Queue setup failed: err=%d, port=%" PRIu8 "\n",
				ret, port_id);
			return -1;
		}
	}

	return 0;
}

/*
 * Callback function for setting mbuf port IDs. Port ID is required by eventev
 * RX adapter and ring pmd doesn't set it automatically.
 */
static uint16_t dpdk_rx_cb(uint16_t port_id, uint16_t queue ODP_UNUSED,
			   struct rte_mbuf *pkts[], uint16_t nb_pkts,
			   uint16_t max_pkts ODP_UNUSED,
			   void *user_param ODP_UNUSED)
{
	uint16_t i;

	for (i = 0; i < nb_pkts; i++)
		pkts[i]->port = port_id;

	return nb_pkts;
}

static void dpdk_ptype_support_set(pktio_entry_t *pktio_entry, uint16_t port_id)
{
	int max_num, num, i;
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	uint32_t mask = RTE_PTYPE_L2_MASK | RTE_PTYPE_L3_MASK |
			RTE_PTYPE_L4_MASK;

	pkt_dpdk->supported_ptypes = 0;

	max_num = rte_eth_dev_get_supported_ptypes(port_id, mask, NULL, 0);
	if (max_num <= 0) {
		ODP_ERR("Device does not support any ptype flags\n");
		return;
	}

	uint32_t ptype[max_num];

	num = rte_eth_dev_get_supported_ptypes(port_id, mask, ptype, max_num);
	if (num <= 0) {
		ODP_ERR("Device does not support any ptype flags\n");
		return;
	}

	for (i = 0; i < num; i++) {
		ODP_DBG("  supported ptype: 0x%x\n", ptype[i]);

		if (ptype[i] == RTE_PTYPE_L2_ETHER_VLAN)
			pkt_dpdk->supported_ptypes |= PTYPE_VLAN;
		else if (ptype[i] == RTE_PTYPE_L2_ETHER_QINQ)
			pkt_dpdk->supported_ptypes |= PTYPE_VLAN_QINQ;
		else if (ptype[i] == RTE_PTYPE_L2_ETHER_ARP)
			pkt_dpdk->supported_ptypes |= PTYPE_ARP;
		else if (RTE_ETH_IS_IPV4_HDR(ptype[i]))
			pkt_dpdk->supported_ptypes |= PTYPE_IPV4;
		else if (RTE_ETH_IS_IPV6_HDR(ptype[i]))
			pkt_dpdk->supported_ptypes |= PTYPE_IPV6;
		else if (ptype[i] == RTE_PTYPE_L4_UDP)
			pkt_dpdk->supported_ptypes |= PTYPE_UDP;
		else if (ptype[i] == RTE_PTYPE_L4_TCP)
			pkt_dpdk->supported_ptypes |= PTYPE_TCP;
	}
}

static int dpdk_start(pktio_entry_t *pktio_entry)
{
	struct rte_eth_dev_info dev_info;
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	uint16_t port_id = pkt_dpdk->port_id;
	int ret;

	if (pktio_entry->s.state == PKTIO_STATE_STOPPED ||
	    pktio_entry->s.state == PKTIO_STATE_STOP_PENDING)
		rte_eth_dev_stop(pkt_dpdk->port_id);

	/* DPDK doesn't support nb_rx_q/nb_tx_q being 0 */
	if (!pktio_entry->s.num_in_queue)
		pktio_entry->s.num_in_queue = 1;
	if (!pktio_entry->s.num_out_queue)
		pktio_entry->s.num_out_queue = 1;

	rte_eth_dev_info_get(port_id, &dev_info);

	/* Setup device */
	if (dpdk_setup_eth_dev(pktio_entry, &dev_info)) {
		ODP_ERR("Failed to configure device\n");
		return -1;
	}

	/* Setup TX queues */
	if (dpdk_setup_eth_tx(pktio_entry, pkt_dpdk, &dev_info))
		return -1;

	/* Setup RX queues */
	if (dpdk_setup_eth_rx(pktio_entry, pkt_dpdk, &dev_info))
		return -1;

	/* Setup promiscuous mode and multicast */
	promisc_mode_check(pkt_dpdk);
	rte_eth_allmulticast_enable(port_id);

	/* Add callback for loopback interface */
	if (pkt_dpdk->loopback) {
		unsigned int i;

		for (i = 0; i < pktio_entry->s.num_in_queue; i++) {
#if RTE_VERSION < RTE_VERSION_NUM(18, 05, 0, 0)
			struct rte_eth_rxtx_callback *callback;
#else
			const struct rte_eth_rxtx_callback *callback;
#endif

			callback = rte_eth_add_rx_callback(port_id, i,
							   dpdk_rx_cb, NULL);
			if (callback == NULL) {
				ODP_ERR("Failed setting RX callback\n");
				return -1;
			}
			pkt_dpdk->rx_callback[i] = callback;
		}
	}

	/* Start device */
	ret = rte_eth_dev_start(port_id);
	if (ret < 0) {
		ODP_ERR("Device start failed: err=%d, port=%" PRIu8 "\n",
			ret, port_id);
		return -1;
	}

	/* Record supported parser ptype flags */
	dpdk_ptype_support_set(pktio_entry, port_id);

	return 0;
}

static int stop_pkt_dpdk(pktio_entry_t *pktio_entry)
{
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);
	unsigned int i;
	uint16_t port_id = pkt_dpdk->port_id;

	for (i = 0; i < pktio_entry->s.num_in_queue; i++)
		rte_eth_dev_rx_queue_stop(port_id, i);
	for (i = 0; i < pktio_entry->s.num_out_queue; i++)
		rte_eth_dev_tx_queue_stop(port_id, i);

	if (!pkt_dpdk->loopback)
		return 0;

	for (i = 0; i < pktio_entry->s.num_in_queue; i++) {
		if (rte_eth_remove_rx_callback(port_id, i,
					       pkt_dpdk->rx_callback[i])) {
			ODP_ERR("Failed removing RX callback\n");
			return -1;
		}
	}
	return 0;
}

static inline void prefetch_pkt(odp_packet_t pkt)
{
	odp_packet_hdr_t *pkt_hdr = packet_hdr(pkt);

	odp_prefetch(&pkt_hdr->p);
}

int input_pkts(pktio_entry_t *pktio_entry, odp_packet_t pkt_table[], int num)
{
	pkt_dpdk_t * const pkt_dpdk = pkt_priv(pktio_entry);
	uint16_t i;
	odp_pktin_config_opt_t pktin_cfg = pktio_entry->s.config.pktin;
	odp_proto_layer_t parse_layer = pktio_entry->s.config.parser.layer;
	odp_pktio_t input = pktio_entry->s.handle;
	odp_time_t ts_val;
	odp_time_t *ts = NULL;
	uint16_t num_prefetch = RTE_MIN(num, NUM_RX_PREFETCH);

	for (i = 0; i < num_prefetch; i++)
		prefetch_pkt(pkt_table[i]);

	if (pktio_entry->s.config.pktin.bit.ts_all ||
	    pktio_entry->s.config.pktin.bit.ts_ptp) {
		ts_val = odp_time_global();
		ts = &ts_val;
	}

	for (i = 0; i < num; ++i) {
		odp_packet_hdr_t *pkt_hdr = packet_hdr(pkt_table[i]);
		struct rte_mbuf *mbuf = pkt_to_mbuf(pkt_table[i]);

		if (odp_likely(i + num_prefetch < num))
			prefetch_pkt(pkt_table[i + num_prefetch]);

		packet_init(pkt_hdr, input);

		if (!pktio_cls_enabled(pktio_entry) &&
		    parse_layer != ODP_PROTO_LAYER_NONE) {
			uint32_t ptypes = pkt_dpdk->supported_ptypes;

			/* DPDK ring pmd doesn't support packet parsing */
			if (pkt_dpdk->loopback) {
				packet_parse_layer(pkt_hdr, parse_layer,
						   pktio_entry->s.in_chksums);
			} else {
				if (_odp_dpdk_packet_parse_layer(pkt_hdr, mbuf,
								 parse_layer,
								 ptypes,
								 pktin_cfg)) {
					odp_packet_free(pkt_table[i]);
					continue;
				}
			}
		}
		packet_set_ts(pkt_hdr, ts);

		odp_prefetch(rte_pktmbuf_mtod(mbuf, char *));
	}

	if (pktio_cls_enabled(pktio_entry)) {
		int failed = 0, success = 0;

		for (i = 0; i < num; i++) {
			odp_packet_t new_pkt;
			odp_packet_t pkt = pkt_table[i];
			odp_pool_t new_pool;
			uint8_t *data;
			odp_packet_hdr_t parsed_hdr;
			odp_packet_hdr_t *pkt_hdr = packet_hdr(pkt);
			struct rte_mbuf *mbuf = pkt_to_mbuf(pkt);
			uint32_t pkt_len = odp_packet_len(pkt);
			uint32_t ptypes = pkt_dpdk->supported_ptypes;

			data = odp_packet_data(pkt);
			packet_parse_reset(&parsed_hdr, 1);
			packet_set_len(&parsed_hdr, pkt_len);

			if (!pkt_dpdk->loopback) {
				int layer = ODP_PROTO_LAYER_ALL;

				if (_odp_dpdk_packet_parse_common(&parsed_hdr.p,
								  data, pkt_len,
								  pkt_len, mbuf,
								  layer, ptypes,
								  pktin_cfg)) {
					odp_packet_free(pkt);
					continue;
				}
			}
			if (cls_classify_packet(pktio_entry, data, pkt_len,
						pkt_len, &new_pool, &parsed_hdr,
						pkt_dpdk->loopback)) {
				failed++;
				odp_packet_free(pkt);
				continue;
			}
			if (new_pool != odp_packet_pool(pkt)) {
				new_pkt = odp_packet_copy(pkt, new_pool);

				odp_packet_free(pkt);
				if (new_pkt == ODP_PACKET_INVALID) {
					failed++;
					continue;
				}
				pkt_table[i] = new_pkt;
				pkt = new_pkt;
				pkt_hdr = packet_hdr(pkt);
			}
			packet_set_ts(pkt_hdr, ts);
			pktio_entry->s.stats.in_octets += odp_packet_len(pkt);
			copy_packet_cls_metadata(&parsed_hdr, pkt_hdr);
			if (success != i)
				pkt_table[success] = pkt;
			++success;
		}
		pktio_entry->s.stats.in_errors += failed;
		pktio_entry->s.stats.in_ucast_pkts += num - failed;
		num = success;
	}

	return num;
}

static int recv_pkt_dpdk(pktio_entry_t *pktio_entry, int index,
			 odp_packet_t pkt_table[], int num)
{
	pkt_dpdk_t * const pkt_dpdk = pkt_priv(pktio_entry);
	pkt_cache_t *rx_cache = &pkt_dpdk->rx_cache[index];
	uint16_t nb_rx;
	uint8_t min_burst = pkt_dpdk->min_rx_burst;
	unsigned int cache_idx;
	int i;

	if (!pkt_dpdk->lockless_rx)
		odp_ticketlock_lock(&pkt_dpdk->rx_lock[index]);

	/**
	 * ixgbe and i40e drivers have a minimum supported RX burst size
	 * ('min_rx_burst'). If 'num' < 'min_rx_burst', 'min_rx_burst' is used
	 * as rte_eth_rx_burst() argument and the possibly received extra
	 * packets are cached for the next dpdk_recv_queue() call to use.
	 *
	 * Either use cached packets or receive new ones. Not both during the
	 * same call. */
	if (odp_unlikely(rx_cache->s.count > 0)) {
		for (i = 0; i < num && rx_cache->s.count; i++) {
			cache_idx = rx_cache->s.idx++;
			pkt_table[i] = (odp_packet_t)rx_cache->s.pkt[cache_idx];
			rx_cache->s.count--;
		}
		nb_rx = i;
	} else if (odp_unlikely((unsigned int)num < min_burst)) {
		struct rte_mbuf *new_mbufs[min_burst];

		nb_rx = rte_eth_rx_burst(pkt_dpdk->port_id, (uint16_t)index,
					 new_mbufs, min_burst);
		rx_cache->s.idx = 0;
		for (i = 0; i < nb_rx; i++) {
			if (i < num) {
				pkt_table[i] = (odp_packet_t)new_mbufs[i];
			} else {
				cache_idx = rx_cache->s.count;
				rx_cache->s.pkt[cache_idx] = new_mbufs[i];
				rx_cache->s.count++;
			}
		}
		nb_rx = RTE_MIN(num, nb_rx);
	} else {
		nb_rx = rte_eth_rx_burst(pkt_dpdk->port_id, (uint16_t)index,
					 (struct rte_mbuf **)pkt_table,
					 (uint16_t)num);
	}

	if (!pkt_dpdk->lockless_rx)
		odp_ticketlock_unlock(&pkt_dpdk->rx_lock[index]);

	/* Packets may also me received through eventdev, so don't add any
	 * processing here. Instead, perform all processing in input_pkts()
	 * which is also called by eventdev. */
	if (nb_rx)
		return input_pkts(pktio_entry, pkt_table, nb_rx);
	return 0;
}

static inline int check_proto(void *l3_hdr, odp_bool_t *l3_proto_v4,
			      uint8_t *l4_proto)
{
	uint8_t l3_proto_ver = _ODP_IPV4HDR_VER(*(uint8_t *)l3_hdr);

	if (l3_proto_ver == _ODP_IPV4) {
		struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)l3_hdr;

		*l3_proto_v4 = 1;
		if (!rte_ipv4_frag_pkt_is_fragmented(ip))
			*l4_proto = ip->next_proto_id;
		else
			*l4_proto = 0;

		return 0;
	} else if (l3_proto_ver == _ODP_IPV6) {
		struct rte_ipv6_hdr *ipv6 = (struct rte_ipv6_hdr *)l3_hdr;

		*l3_proto_v4 = 0;
		*l4_proto = ipv6->proto;
		return 0;
	}

	return -1;
}

static inline uint16_t phdr_csum(odp_bool_t ipv4, void *l3_hdr,
				 uint64_t ol_flags)
{
	if (ipv4)
		return rte_ipv4_phdr_cksum(l3_hdr, ol_flags);
	else /*ipv6*/
		return rte_ipv6_phdr_cksum(l3_hdr, ol_flags);
}

#define OL_TX_CHKSUM_PKT(_cfg, _capa, _proto, _ovr_set, _ovr) \
	(_capa && _proto && (_ovr_set ? _ovr : _cfg))

static inline void pkt_set_ol_tx(odp_pktout_config_opt_t *pktout_cfg,
				 odp_pktout_config_opt_t *pktout_capa,
				 odp_packet_hdr_t *pkt_hdr,
				 struct rte_mbuf *mbuf,
				 char *mbuf_data)
{
	void *l3_hdr, *l4_hdr;
	uint8_t l4_proto;
	odp_bool_t l3_proto_v4;
	odp_bool_t ipv4_chksum_pkt, udp_chksum_pkt, tcp_chksum_pkt;
	packet_parser_t *pkt_p = &pkt_hdr->p;

	if (pkt_p->l3_offset == ODP_PACKET_OFFSET_INVALID)
		return;

	l3_hdr = (void *)(mbuf_data + pkt_p->l3_offset);

	if (check_proto(l3_hdr, &l3_proto_v4, &l4_proto))
		return;

	ipv4_chksum_pkt = OL_TX_CHKSUM_PKT(pktout_cfg->bit.ipv4_chksum,
					   pktout_capa->bit.ipv4_chksum,
					   l3_proto_v4,
					   pkt_p->flags.l3_chksum_set,
					   pkt_p->flags.l3_chksum);
	udp_chksum_pkt =  OL_TX_CHKSUM_PKT(pktout_cfg->bit.udp_chksum,
					   pktout_capa->bit.udp_chksum,
					   (l4_proto == _ODP_IPPROTO_UDP),
					   pkt_p->flags.l4_chksum_set,
					   pkt_p->flags.l4_chksum);
	tcp_chksum_pkt =  OL_TX_CHKSUM_PKT(pktout_cfg->bit.tcp_chksum,
					   pktout_capa->bit.tcp_chksum,
					   (l4_proto == _ODP_IPPROTO_TCP),
					   pkt_p->flags.l4_chksum_set,
					   pkt_p->flags.l4_chksum);

	if (!ipv4_chksum_pkt && !udp_chksum_pkt && !tcp_chksum_pkt)
		return;

	mbuf->l2_len = pkt_p->l3_offset - pkt_p->l2_offset;

	if (l3_proto_v4)
		mbuf->ol_flags = PKT_TX_IPV4;
	else
		mbuf->ol_flags = PKT_TX_IPV6;

	if (ipv4_chksum_pkt) {
		mbuf->ol_flags |=  PKT_TX_IP_CKSUM;

		((struct rte_ipv4_hdr *)l3_hdr)->hdr_checksum = 0;
		mbuf->l3_len = _ODP_IPV4HDR_IHL(*(uint8_t *)l3_hdr) * 4;
	}

	if (pkt_p->l4_offset == ODP_PACKET_OFFSET_INVALID)
		return;

	mbuf->l3_len = pkt_p->l4_offset - pkt_p->l3_offset;

	l4_hdr = (void *)(mbuf_data + pkt_p->l4_offset);

	if (udp_chksum_pkt) {
		mbuf->ol_flags |= PKT_TX_UDP_CKSUM;

		((struct rte_udp_hdr *)l4_hdr)->dgram_cksum =
			phdr_csum(l3_proto_v4, l3_hdr, mbuf->ol_flags);
	} else if (tcp_chksum_pkt) {
		mbuf->ol_flags |= PKT_TX_TCP_CKSUM;

		((struct rte_tcp_hdr *)l4_hdr)->cksum =
			phdr_csum(l3_proto_v4, l3_hdr, mbuf->ol_flags);
	}
}

static int send_pkt_dpdk(pktio_entry_t *pktio_entry, int index,
			 const odp_packet_t pkt_table[], int num)
{
	pkt_dpdk_t * const pkt_dpdk = pkt_priv(pktio_entry);
	uint8_t chksum_insert_ena = pktio_entry->s.chksum_insert_ena;
	odp_pktout_config_opt_t *pktout_cfg = &pktio_entry->s.config.pktout;
	odp_pktout_config_opt_t *pktout_capa =
		&pktio_entry->s.capa.config.pktout;
	int pkts;

	if (chksum_insert_ena) {
		int i;

		for (i = 0; i < num; i++) {
			struct rte_mbuf *mbuf = pkt_to_mbuf(pkt_table[i]);

			pkt_set_ol_tx(pktout_cfg, pktout_capa,
				      packet_hdr(pkt_table[i]), mbuf,
				      rte_pktmbuf_mtod(mbuf, char *));
		}
	}

	if (!pkt_dpdk->lockless_tx)
		odp_ticketlock_lock(&pkt_dpdk->tx_lock[index]);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	pkts = rte_eth_tx_burst(pkt_dpdk->port_id, index,
				(struct rte_mbuf **)pkt_table, num);
#pragma GCC diagnostic pop

	if (!pkt_dpdk->lockless_tx)
		odp_ticketlock_unlock(&pkt_dpdk->tx_lock[index]);

	if (pkts == 0) {
		struct rte_mbuf *mbuf = pkt_to_mbuf(pkt_table[0]);

		if (odp_unlikely(rte_errno != 0))
			return -1;

		if (odp_unlikely(mbuf->pkt_len > pkt_dpdk->mtu)) {
			__odp_errno = EMSGSIZE;
			return -1;
		}
	}
	rte_errno = 0;
	return pkts;
}

static uint32_t _dpdk_vdev_mtu(uint16_t port_id)
{
	struct rte_eth_dev_info dev_info;
	struct ifreq ifr;
	int ret;
	int sockfd;

	rte_eth_dev_info_get(port_id, &dev_info);
	if_indextoname(dev_info.if_index, ifr.ifr_name);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ioctl(sockfd, SIOCGIFMTU, &ifr);
	close(sockfd);
	if (ret < 0) {
		ODP_DBG("ioctl SIOCGIFMTU error\n");
		return 0;
	}

	return ifr.ifr_mtu;
}

static uint32_t mtu_get_pkt_dpdk(pktio_entry_t *pktio_entry)
{
	uint16_t mtu = 0;
	int ret;

	ret = rte_eth_dev_get_mtu(pkt_priv(pktio_entry)->port_id, &mtu);
	if (ret < 0)
		return 0;

	/* some dpdk PMD vdev does not support getting mtu size,
	 * try to use system call if dpdk cannot get mtu value.
	 */
	if (mtu == 0)
		mtu = _dpdk_vdev_mtu(pkt_priv(pktio_entry)->port_id);
	return mtu;
}

static uint32_t dpdk_frame_maxlen(pktio_entry_t *pktio_entry)
{
	pkt_dpdk_t *pkt_dpdk = pkt_priv(pktio_entry);

	return pkt_dpdk->mtu;
}

static int _dpdk_vdev_promisc_mode_set(uint16_t port_id, int enable)
{
	struct rte_eth_dev_info dev_info;
	struct ifreq ifr;
	int ret;
	int sockfd;

	rte_eth_dev_info_get(port_id, &dev_info);
	if_indextoname(dev_info.if_index, ifr.ifr_name);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	ret = ioctl(sockfd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		close(sockfd);
		ODP_DBG("ioctl SIOCGIFFLAGS error\n");
		return -1;
	}

	if (enable)
		ifr.ifr_flags |= IFF_PROMISC;
	else
		ifr.ifr_flags &= ~(IFF_PROMISC);

	ret = ioctl(sockfd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		close(sockfd);
		ODP_DBG("ioctl SIOCSIFFLAGS error\n");
		return -1;
	}

	ret = ioctl(sockfd, SIOCGIFMTU, &ifr);
	if (ret < 0) {
		close(sockfd);
		ODP_DBG("ioctl SIOCGIFMTU error\n");
		return -1;
	}

	ODP_DBG("vdev promisc set to %d\n", enable);
	close(sockfd);
	return 0;
}

static int promisc_mode_set_pkt_dpdk(pktio_entry_t *pktio_entry,  int enable)
{
	uint16_t port_id = pkt_priv(pktio_entry)->port_id;

	if (enable)
		rte_eth_promiscuous_enable(port_id);
	else
		rte_eth_promiscuous_disable(port_id);

	if (pkt_priv(pktio_entry)->vdev_sysc_promisc) {
		int ret = _dpdk_vdev_promisc_mode_set(port_id, enable);

		if (ret < 0)
			ODP_DBG("vdev promisc mode fail\n");
	}

	return 0;
}

static int _dpdk_vdev_promisc_mode(uint16_t port_id)
{
	struct rte_eth_dev_info dev_info;
	struct ifreq ifr;
	int ret;
	int sockfd;

	rte_eth_dev_info_get(port_id, &dev_info);
	if_indextoname(dev_info.if_index, ifr.ifr_name);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ioctl(sockfd, SIOCGIFFLAGS, &ifr);
	close(sockfd);
	if (ret < 0) {
		ODP_DBG("ioctl SIOCGIFFLAGS error\n");
		return -1;
	}

	if (ifr.ifr_flags & IFF_PROMISC) {
		ODP_DBG("promisc is 1\n");
		return 1;
	} else {
		return 0;
	}
}

static int promisc_mode_get_pkt_dpdk(pktio_entry_t *pktio_entry)
{
	pkt_dpdk_t * const pkt_dpdk = pkt_priv(pktio_entry);
	uint16_t port_id = pkt_dpdk->port_id;

	/* Loopback interface always in promisc mode */
	if (pkt_dpdk->loopback)
		return 1;

	if (pkt_dpdk->vdev_sysc_promisc)
		return _dpdk_vdev_promisc_mode(port_id);
	else
		return rte_eth_promiscuous_get(port_id);
}

static int mac_get_pkt_dpdk(pktio_entry_t *pktio_entry, void *mac_addr)
{
	rte_eth_macaddr_get(pkt_priv(pktio_entry)->port_id,
			    (struct rte_ether_addr *)mac_addr);
	return ETH_ALEN;
}

static int mac_set_pkt_dpdk(pktio_entry_t *pktio_entry, const void *mac_addr)
{
	struct rte_ether_addr addr = *(const struct rte_ether_addr *)mac_addr;

	return rte_eth_dev_default_mac_addr_set(pkt_priv(pktio_entry)->port_id,
						&addr);
}

static int capability_pkt_dpdk(pktio_entry_t *pktio_entry,
			       odp_pktio_capability_t *capa)
{
	*capa = pktio_entry->s.capa;
	return 0;
}

static int link_status_pkt_dpdk(pktio_entry_t *pktio_entry)
{
	struct rte_eth_link link;

	rte_eth_link_get(pkt_priv(pktio_entry)->port_id, &link);
	return link.link_status;
}

static void stats_convert(struct rte_eth_stats *rte_stats,
			  odp_pktio_stats_t *stats)
{
	stats->in_octets = rte_stats->ibytes;
	stats->in_ucast_pkts = 0;
	stats->in_discards = rte_stats->imissed;
	stats->in_errors = rte_stats->ierrors;
	stats->in_unknown_protos = 0;
	stats->out_octets = rte_stats->obytes;
	stats->out_ucast_pkts = 0;
	stats->out_discards = 0;
	stats->out_errors = rte_stats->oerrors;
}

static int stats_pkt_dpdk(pktio_entry_t *pktio_entry, odp_pktio_stats_t *stats)
{
	int ret;
	struct rte_eth_stats rte_stats;

	ret = rte_eth_stats_get(pkt_priv(pktio_entry)->port_id, &rte_stats);

	if (ret == 0) {
		stats_convert(&rte_stats, stats);
		return 0;
	}

	if (ret > 0)
		return -ret;
	else
		return ret;
}

static int stats_reset_pkt_dpdk(pktio_entry_t *pktio_entry)
{
	rte_eth_stats_reset(pkt_priv(pktio_entry)->port_id);
	return 0;
}

const pktio_if_ops_t dpdk_pktio_ops = {
	.name = "odp-dpdk",
	.print = NULL,
	.init_global = dpdk_init_global,
	.init_local = NULL,
	.term = dpdk_term_global,
	.open = setup_pkt_dpdk,
	.close = close_pkt_dpdk,
	.start = dpdk_start,
	.stop = stop_pkt_dpdk,
	.stats = stats_pkt_dpdk,
	.stats_reset = stats_reset_pkt_dpdk,
	.pktin_ts_res = NULL,
	.pktin_ts_from_ns = NULL,
	.mtu_get = dpdk_frame_maxlen,
	.promisc_mode_set = promisc_mode_set_pkt_dpdk,
	.promisc_mode_get = promisc_mode_get_pkt_dpdk,
	.mac_get = mac_get_pkt_dpdk,
	.mac_set = mac_set_pkt_dpdk,
	.link_status = link_status_pkt_dpdk,
	.capability = capability_pkt_dpdk,
	.config = NULL,
	.input_queues_config = dpdk_input_queues_config,
	.output_queues_config = dpdk_output_queues_config,
	.recv = recv_pkt_dpdk,
	.send = send_pkt_dpdk
};
