var group__odp__packet__io =
[
    [ "odp_pktin_queue_t", "structodp__pktin__queue__t.html", [
      [ "pktio", "structodp__pktin__queue__t.html#ad5da010b573dab52234a0c90e6cb7583", null ],
      [ "index", "structodp__pktin__queue__t.html#a396034547deb1f3f94b3a05d4f6f4341", null ]
    ] ],
    [ "odp_pktout_queue_t", "structodp__pktout__queue__t.html", [
      [ "pktio", "structodp__pktout__queue__t.html#a8d402188a4f9f02cbbcfb1587b9c3787", null ],
      [ "index", "structodp__pktout__queue__t.html#ae513ce2a982c8f4fc3c8369b9ae8523f", null ]
    ] ],
    [ "odp_pktin_hash_proto_t", "unionodp__pktin__hash__proto__t.html", [
      [ "ipv4_udp", "unionodp__pktin__hash__proto__t.html#a55c2478785f04914939ed859bdc16839", null ],
      [ "ipv4_tcp", "unionodp__pktin__hash__proto__t.html#a268c94e707aca11118db930f5f00d103", null ],
      [ "ipv4", "unionodp__pktin__hash__proto__t.html#ade3193ac0eb548aac8eb5001fd0933f8", null ],
      [ "ipv6_udp", "unionodp__pktin__hash__proto__t.html#ad379b01fb0ab239903ea88204c838862", null ],
      [ "ipv6_tcp", "unionodp__pktin__hash__proto__t.html#a2824b7ed8f136060f95a35aed6259f35", null ],
      [ "ipv6", "unionodp__pktin__hash__proto__t.html#a98e3770479e4ec255ecc58fb0f5ca4dd", null ],
      [ "proto", "unionodp__pktin__hash__proto__t.html#ac78aac8fd370bcd9065b32a85c5a694b", null ],
      [ "all_bits", "unionodp__pktin__hash__proto__t.html#a12d95f38ba5bf51187f03dfc01fab978", null ]
    ] ],
    [ "odp_pktin_queue_param_ovr_t", "structodp__pktin__queue__param__ovr__t.html", [
      [ "group", "structodp__pktin__queue__param__ovr__t.html#a459c97f3edac93e48821d652d81b1029", null ]
    ] ],
    [ "odp_pktin_vector_config_t", "structodp__pktin__vector__config__t.html", [
      [ "enable", "structodp__pktin__vector__config__t.html#a0491d32f09584e60a1ba9c3dfc8754fe", null ],
      [ "pool", "structodp__pktin__vector__config__t.html#ad842cadcf8f34dc34d58d5dddc32f098", null ],
      [ "max_tmo_ns", "structodp__pktin__vector__config__t.html#a4101840165086a1e6d18c522fe078d1c", null ],
      [ "max_size", "structodp__pktin__vector__config__t.html#a24aebe4425f0743eeb12d51dad6a06cb", null ]
    ] ],
    [ "odp_pktin_queue_param_t", "structodp__pktin__queue__param__t.html", [
      [ "op_mode", "structodp__pktin__queue__param__t.html#a1a15c51c24187f0f12e6482761ceb553", null ],
      [ "classifier_enable", "structodp__pktin__queue__param__t.html#ad01b006a1bd0580ab3898d8184ab7872", null ],
      [ "hash_enable", "structodp__pktin__queue__param__t.html#aa90bfc914603a90f7b1f7e7f0de07418", null ],
      [ "hash_proto", "structodp__pktin__queue__param__t.html#a901146fbd15779b5665d3b75f3488e91", null ],
      [ "num_queues", "structodp__pktin__queue__param__t.html#a2d6b721acb69bb9bceabf063626a4024", null ],
      [ "queue_param", "structodp__pktin__queue__param__t.html#a5a07d5e93f26a523103360c0bea4a304", null ],
      [ "queue_param_ovr", "structodp__pktin__queue__param__t.html#a868bc45745fe32633e4e2ffdb4435b0c", null ],
      [ "vector", "structodp__pktin__queue__param__t.html#ad4ee17129828acc77dbb947f93860e17", null ]
    ] ],
    [ "odp_pktout_queue_param_t", "structodp__pktout__queue__param__t.html", [
      [ "op_mode", "structodp__pktout__queue__param__t.html#a066fd0a2de0efca0056cd1529d78b06e", null ],
      [ "num_queues", "structodp__pktout__queue__param__t.html#a3d8868548c1bc96c6dda574d94456e83", null ]
    ] ],
    [ "odp_pktio_param_t", "structodp__pktio__param__t.html", [
      [ "in_mode", "structodp__pktio__param__t.html#a819cf5e2ef7df43a729cad159c022f4d", null ],
      [ "out_mode", "structodp__pktio__param__t.html#afc226fd3904618950a0d1154ffa2fd8c", null ]
    ] ],
    [ "odp_pktin_config_opt_t", "unionodp__pktin__config__opt__t.html", [
      [ "ts_all", "unionodp__pktin__config__opt__t.html#a0955307af97f20d4e8676934c999387c", null ],
      [ "ts_ptp", "unionodp__pktin__config__opt__t.html#a09c131911d71a77dee93c58ee4ef05bb", null ],
      [ "ipv4_chksum", "unionodp__pktin__config__opt__t.html#a33b8e4e0d87ab893425e965cff729170", null ],
      [ "udp_chksum", "unionodp__pktin__config__opt__t.html#a6a0d225bf79294d554f7ff0db44410cb", null ],
      [ "tcp_chksum", "unionodp__pktin__config__opt__t.html#ac88448a925b1a05f0cb252adf493fcfe", null ],
      [ "sctp_chksum", "unionodp__pktin__config__opt__t.html#aedb9fb8513c14219cb600bf6b91974f6", null ],
      [ "drop_ipv4_err", "unionodp__pktin__config__opt__t.html#ac239eee008853f3f0eb0c0544d567025", null ],
      [ "drop_ipv6_err", "unionodp__pktin__config__opt__t.html#afe657ca9ac5ce9ee0cbb27263b39eb2e", null ],
      [ "drop_udp_err", "unionodp__pktin__config__opt__t.html#a60b6136328df76ec1d48ba6bb08378e3", null ],
      [ "drop_tcp_err", "unionodp__pktin__config__opt__t.html#aff3d2a2d44cb7975cf8c53684f9b3eb3", null ],
      [ "drop_sctp_err", "unionodp__pktin__config__opt__t.html#a48a5c4442355756b5956fefdb0270de5", null ],
      [ "bit", "unionodp__pktin__config__opt__t.html#aaefe012b91f97e6f6d0591486368684b", null ],
      [ "all_bits", "unionodp__pktin__config__opt__t.html#a389e14458b86f85d03812da033c56e96", null ]
    ] ],
    [ "odp_pktout_config_opt_t", "unionodp__pktout__config__opt__t.html", [
      [ "ts_ena", "unionodp__pktout__config__opt__t.html#a9e6582a0ff0841424551b2e651f0fe74", null ],
      [ "ipv4_chksum_ena", "unionodp__pktout__config__opt__t.html#ab4f8abd7f5f4c4c828c3dbae3c35133c", null ],
      [ "udp_chksum_ena", "unionodp__pktout__config__opt__t.html#ae2165ea403b5ea30ae31d6903a46b3d4", null ],
      [ "tcp_chksum_ena", "unionodp__pktout__config__opt__t.html#abc946c3da901662466eb6586050aef7e", null ],
      [ "sctp_chksum_ena", "unionodp__pktout__config__opt__t.html#a2bcfd8088641b1ab5c420a62a436266c", null ],
      [ "ipv4_chksum", "unionodp__pktout__config__opt__t.html#aa1a27e50cf6f729511624c1467dd0816", null ],
      [ "udp_chksum", "unionodp__pktout__config__opt__t.html#aceca46f8087643a5c291ec73165f020c", null ],
      [ "tcp_chksum", "unionodp__pktout__config__opt__t.html#a5825269d8ec1fc366837ffe446b49508", null ],
      [ "sctp_chksum", "unionodp__pktout__config__opt__t.html#a8c0db3340e79873add9388cfc2f8a1bf", null ],
      [ "no_packet_refs", "unionodp__pktout__config__opt__t.html#a81884b76374c6af17d59b8d852339f4e", null ],
      [ "aging_ena", "unionodp__pktout__config__opt__t.html#a244b6113231315b7af72b0953c224b3a", null ],
      [ "tx_compl_ena", "unionodp__pktout__config__opt__t.html#a8f4d228ca0719dd556b37ef5980d265f", null ],
      [ "bit", "unionodp__pktout__config__opt__t.html#aa2136d86af65fffb026dae63aa23fe6c", null ],
      [ "all_bits", "unionodp__pktout__config__opt__t.html#a93e089faddd1d159b541e95fc819b464", null ]
    ] ],
    [ "odp_pktio_parser_config_t", "structodp__pktio__parser__config__t.html", [
      [ "layer", "structodp__pktio__parser__config__t.html#aff1f0d93e1f3cc93006aa613287378a2", null ]
    ] ],
    [ "odp_pktio_config_t", "structodp__pktio__config__t.html", [
      [ "pktin", "structodp__pktio__config__t.html#ad8e059ec7e784ef0f2d6ffc6623620b7", null ],
      [ "pktout", "structodp__pktio__config__t.html#a5f262ff7d35dc46c326322407867fe85", null ],
      [ "parser", "structodp__pktio__config__t.html#ab11707ba4258066a50f0e029fcc641f5", null ],
      [ "enable_loop", "structodp__pktio__config__t.html#a86118ff96a26bd323e5d47abbf702e8d", null ],
      [ "inbound_ipsec", "structodp__pktio__config__t.html#ae5e0a272a734daa48cf404e740074f5d", null ],
      [ "outbound_ipsec", "structodp__pktio__config__t.html#ac49be9c22b1aad5293c3e2d4e1a4a629", null ],
      [ "enable_lso", "structodp__pktio__config__t.html#aadb671784a5944f7e7ee75913a7f30b5", null ],
      [ "reassembly", "structodp__pktio__config__t.html#a34a55e98dcf0a7e341cf0a572fc30fe1", null ]
    ] ],
    [ "odp_pktio_set_op_t", "unionodp__pktio__set__op__t.html", [
      [ "promisc_mode", "unionodp__pktio__set__op__t.html#ae821914c6895a2cfbea766f6c70d48bc", null ],
      [ "mac_addr", "unionodp__pktio__set__op__t.html#aefe91e13713b2d91ce00df4e61853c52", null ],
      [ "skip_offset", "unionodp__pktio__set__op__t.html#ad866cfc1a21f13107d2283ff2c38312a", null ],
      [ "maxlen", "unionodp__pktio__set__op__t.html#a47fb7792a712983f7a670e24ef9cd08a", null ],
      [ "op", "unionodp__pktio__set__op__t.html#ac6424570fb6dd9e4a504c43987df92db", null ],
      [ "all_bits", "unionodp__pktio__set__op__t.html#a6aacfd8302c0bf3dbdcb91250b961ac6", null ]
    ] ],
    [ "odp_lso_capability_t", "structodp__lso__capability__t.html", [
      [ "max_profiles", "structodp__lso__capability__t.html#a2a3f6fd794bd4af8f5e0edbdc8c940cc", null ],
      [ "max_profiles_per_pktio", "structodp__lso__capability__t.html#a45bb6eea4afb600555e380a637e9043b", null ],
      [ "max_packet_segments", "structodp__lso__capability__t.html#a1d7b085ce40fc7b9079af086e9cd7446", null ],
      [ "max_segments", "structodp__lso__capability__t.html#a230855159613f4ffeb476d72cb974d9d", null ],
      [ "max_payload_len", "structodp__lso__capability__t.html#a136795cdaf6c3d4fc424288aec7979a4", null ],
      [ "max_payload_offset", "structodp__lso__capability__t.html#a9156d95287fe3f2ab6a2503c37831465", null ],
      [ "add_segment_num", "structodp__lso__capability__t.html#a80f0a98d0464ee162274c9be9af7daec", null ],
      [ "add_payload_len", "structodp__lso__capability__t.html#a60cb95c5d449b8cac4bce2216c656a14", null ],
      [ "add_payload_offset", "structodp__lso__capability__t.html#ae1af855ef1d39d1218956b3f1261c83a", null ],
      [ "mod_op", "structodp__lso__capability__t.html#a27190f3bc80a9d7ecc89799daf29e84e", null ],
      [ "max_num_custom", "structodp__lso__capability__t.html#a68bcb268606f7e19248fa33c3f66c7ca", null ],
      [ "custom", "structodp__lso__capability__t.html#ab2f71345c6bef47c764ea83418fee732", null ],
      [ "ipv4", "structodp__lso__capability__t.html#a914f4b5b5226fba244d741f9b2c3c801", null ],
      [ "ipv6", "structodp__lso__capability__t.html#adc81d409323a3482bae0ef9aed951e02", null ],
      [ "tcp_ipv4", "structodp__lso__capability__t.html#a7b11dcd425c6633b4a2145d52de74840", null ],
      [ "tcp_ipv6", "structodp__lso__capability__t.html#ab0b3a7196d03f728b51cc94535fcf391", null ],
      [ "sctp_ipv4", "structodp__lso__capability__t.html#a2d62cdce1c17ac4a0fdadc60f9dca5c9", null ],
      [ "sctp_ipv6", "structodp__lso__capability__t.html#ad1685032ab710ad8b151fbcfc19621ba", null ],
      [ "proto", "structodp__lso__capability__t.html#a34956cfa69becb87211b8164ef529c01", null ]
    ] ],
    [ "odp_pktin_vector_capability_t", "structodp__pktin__vector__capability__t.html", [
      [ "supported", "structodp__pktin__vector__capability__t.html#ab6a4470a161ae515c11a63ddb11ecd34", null ],
      [ "max_size", "structodp__pktin__vector__capability__t.html#a556ccbaee9fddbba81b63bc8db760030", null ],
      [ "min_size", "structodp__pktin__vector__capability__t.html#a4bff2d61ffb90a45984dbde710c85b2a", null ],
      [ "max_tmo_ns", "structodp__pktin__vector__capability__t.html#a0960610b96e48336a20826f96f3530b6", null ],
      [ "min_tmo_ns", "structodp__pktin__vector__capability__t.html#a20ddfe06a13f31faa76f557f8bf94120", null ]
    ] ],
    [ "odp_pktio_capability_t", "structodp__pktio__capability__t.html", [
      [ "max_input_queues", "structodp__pktio__capability__t.html#a5cd6fa01b3c1240cbe5be871ea148b23", null ],
      [ "max_output_queues", "structodp__pktio__capability__t.html#afb0d1efa99f21c620e45535ddccb042e", null ],
      [ "config", "structodp__pktio__capability__t.html#a566eb4053dc5ace3aa11092f3d284aea", null ],
      [ "set_op", "structodp__pktio__capability__t.html#a2d4329604c32dda9539d9ea9831b2291", null ],
      [ "loop_supported", "structodp__pktio__capability__t.html#a4bd806eca0438affd5dc2b20f1f4d366", null ],
      [ "vector", "structodp__pktio__capability__t.html#a3d4c8800b73de78c5d95946b64b0105a", null ],
      [ "lso", "structodp__pktio__capability__t.html#acec4778cab5fd7564537840df68c4992", null ],
      [ "equal", "structodp__pktio__capability__t.html#a19a9584b006ee2f032f5da28b82f6d10", null ],
      [ "min_input", "structodp__pktio__capability__t.html#aac6facf8d541758a0d336dc6e8842438", null ],
      [ "max_input", "structodp__pktio__capability__t.html#a5243a96efbd4561947e2dd6ebf00a1dc", null ],
      [ "min_output", "structodp__pktio__capability__t.html#a741be7a79f65ef773e6bb9cbf305eb4c", null ],
      [ "max_output", "structodp__pktio__capability__t.html#a2f6c0f1389d93f2da0be4a14f8a1fd5b", null ],
      [ "maxlen", "structodp__pktio__capability__t.html#a6493c91523f3fa364fbda9d5fa49cf20", null ],
      [ "max_tx_aging_tmo_ns", "structodp__pktio__capability__t.html#a9ac97decbfc7d6f457f9cedd973d6220", null ],
      [ "queue_type_sched", "structodp__pktio__capability__t.html#a10cc7a2afc62885811ad51ea3a6529ac", null ],
      [ "queue_type_plain", "structodp__pktio__capability__t.html#aea36218c58e53d8739b572911fe85bbc", null ],
      [ "mode_all", "structodp__pktio__capability__t.html#a377ef11151fa7aebaea22980b119020e", null ],
      [ "tx_compl", "structodp__pktio__capability__t.html#a8e2683e20a63ce6ac2e6aeb4a8a2f2fc", null ],
      [ "reassembly", "structodp__pktio__capability__t.html#afd6b982854f943e8e5070c675ef8cf69", null ]
    ] ],
    [ "odp_lso_profile_param_t", "structodp__lso__profile__param__t.html", [
      [ "lso_proto", "structodp__lso__profile__param__t.html#a0a07267d6e6d8128e21b892f1777df5c", null ],
      [ "mod_op", "structodp__lso__profile__param__t.html#ae9e0899ef5bf20799bbea23c3b995069", null ],
      [ "offset", "structodp__lso__profile__param__t.html#a20a4789d745e16537cdc15a3b39aec5f", null ],
      [ "size", "structodp__lso__profile__param__t.html#aed77ca6ada3e260755c6f0c33344346a", null ],
      [ "field", "structodp__lso__profile__param__t.html#a3cd3b2f141b3dde374f82eb94e2afc59", null ],
      [ "num_custom", "structodp__lso__profile__param__t.html#a0858b406b0e0033bef8b675d8197b7dd", null ],
      [ "custom", "structodp__lso__profile__param__t.html#a0317b7a52c13b2c5aee947cf98459558", null ]
    ] ],
    [ "odp_pktio_info_t", "structodp__pktio__info__t.html", [
      [ "name", "structodp__pktio__info__t.html#ab821dfd9c69a437bee30e2ed8bef8845", null ],
      [ "drv_name", "structodp__pktio__info__t.html#a04b876751a7d60c1f333dd9df58b9ecc", null ],
      [ "pool", "structodp__pktio__info__t.html#aa7d1901ce13f6f971f5d87a0f72809d8", null ],
      [ "param", "structodp__pktio__info__t.html#a2d944df4faf8fabf208041be185a174a", null ]
    ] ],
    [ "odp_pktio_link_info_t", "structodp__pktio__link__info__t.html", [
      [ "autoneg", "structodp__pktio__link__info__t.html#a7e45e18d5c7c201090baf8f36cd1ec89", null ],
      [ "duplex", "structodp__pktio__link__info__t.html#a2fdd99b05ad2b2be2c2d4b31aec7302a", null ],
      [ "media", "structodp__pktio__link__info__t.html#aa7e8fd64e25cbdcdc41952cfeb7e23f2", null ],
      [ "pause_rx", "structodp__pktio__link__info__t.html#a035b6dc2f49377750d4d4e5a665d4331", null ],
      [ "pause_tx", "structodp__pktio__link__info__t.html#aa2797270542790c833319ff4a7572aee", null ],
      [ "speed", "structodp__pktio__link__info__t.html#a4c295498f4be01ed398c0a1092ae6f2a", null ],
      [ "status", "structodp__pktio__link__info__t.html#abcfbbfd2c5c131fb28a6cefb215c1319", null ]
    ] ],
    [ "odp_pktio_stats_t", "structodp__pktio__stats__t.html", [
      [ "in_octets", "structodp__pktio__stats__t.html#a47fe93dcbaa563ac7ef61cced69264f7", null ],
      [ "in_packets", "structodp__pktio__stats__t.html#ac7c23485e42928595b242339019d9e60", null ],
      [ "in_ucast_pkts", "structodp__pktio__stats__t.html#a34fd2b3a9b9d2c5144a6e2578e34e74c", null ],
      [ "in_discards", "structodp__pktio__stats__t.html#a5d70dd28754c380b0615c6305a4b67b0", null ],
      [ "in_errors", "structodp__pktio__stats__t.html#a4533bd0a6d0668569b6c26a0e7578caf", null ],
      [ "in_unknown_protos", "structodp__pktio__stats__t.html#a0dce262c67ec2eccd4a7c29fbf5b6ef8", null ],
      [ "out_octets", "structodp__pktio__stats__t.html#a3df3890e04b16a24b9183aac56a22b69", null ],
      [ "out_packets", "structodp__pktio__stats__t.html#aee03783e07426a7e22cd2fe5ca43a3fe", null ],
      [ "out_ucast_pkts", "structodp__pktio__stats__t.html#a2f9b7cd41c152c6a744191f6d8e8182e", null ],
      [ "out_discards", "structodp__pktio__stats__t.html#a6eb817e59b52f4ff89a5d2e5dc82ccd8", null ],
      [ "out_errors", "structodp__pktio__stats__t.html#a9d51a063c3dea82f8ad82a770d6e7f8f", null ]
    ] ],
    [ "ODP_PKTIO_INVALID", "group__odp__packet__io.html#ga9c7d0fcaec232b5503633e42ea29f6f5", null ],
    [ "ODP_LSO_PROFILE_INVALID", "group__odp__packet__io.html#ga3e1b36637a63ca883135394c546d98b7", null ],
    [ "ODP_PKTIO_MACADDR_MAXSIZE", "group__odp__packet__io.html#ga2cbb98393b188166e12fa84bc919de4a", null ],
    [ "ODP_PKTIN_NO_WAIT", "group__odp__packet__io.html#ga7502d5a4734fd721ee86a2ad937aee88", null ],
    [ "ODP_PKTIO_PARSER_LAYER_NONE", "group__odp__packet__io.html#ga8cb8ad8f5ccdf9f29783b39c54815984", null ],
    [ "ODP_PKTIO_PARSER_LAYER_L2", "group__odp__packet__io.html#gac0bfb759e7c2b1722701e3b3d6acd511", null ],
    [ "ODP_PKTIO_PARSER_LAYER_L3", "group__odp__packet__io.html#gae24cd60b678ef1b2886530f4ef26ae76", null ],
    [ "ODP_PKTIO_PARSER_LAYER_L4", "group__odp__packet__io.html#gad19445606f602b67124205f60305f7dd", null ],
    [ "ODP_PKTIO_PARSER_LAYER_ALL", "group__odp__packet__io.html#ga65815b8a3cea24610cbbcdb6acda3ed7", null ],
    [ "ODP_LSO_MAX_CUSTOM", "group__odp__packet__io.html#ga4df3f8e3638eeca1bb1e0bc5295f60f0", null ],
    [ "ODP_PKTIO_LINK_SPEED_UNKNOWN", "group__odp__packet__io.html#gab305b1c11c90fcd307ca5dd3da86b4a9", null ],
    [ "ODP_PKTIO_LINK_SPEED_10M", "group__odp__packet__io.html#gabb3945cbf0cc0d92877b14a92e4a994c", null ],
    [ "ODP_PKTIO_LINK_SPEED_100M", "group__odp__packet__io.html#ga456dc07603bbd66c2d3f3c22c350dcb7", null ],
    [ "ODP_PKTIO_LINK_SPEED_1G", "group__odp__packet__io.html#gae829973ca3d4d90dcaa9ab63191ef0d3", null ],
    [ "ODP_PKTIO_LINK_SPEED_2_5G", "group__odp__packet__io.html#gaefdbbbf5ec7d70e20d706101b4f5f79f", null ],
    [ "ODP_PKTIO_LINK_SPEED_5G", "group__odp__packet__io.html#ga8b0c0b802599082e5500c965d19bd5f8", null ],
    [ "ODP_PKTIO_LINK_SPEED_10G", "group__odp__packet__io.html#ga97309b4a542241cb751f56bded804fc6", null ],
    [ "ODP_PKTIO_LINK_SPEED_20G", "group__odp__packet__io.html#gad8416fcdccbf67a6ba53a45dbc0fe294", null ],
    [ "ODP_PKTIO_LINK_SPEED_25G", "group__odp__packet__io.html#gaa0ccd378dbff16c0444f94e856f56110", null ],
    [ "ODP_PKTIO_LINK_SPEED_40G", "group__odp__packet__io.html#gaf46e2ed5d2f82e44af5701c721009279", null ],
    [ "ODP_PKTIO_LINK_SPEED_50G", "group__odp__packet__io.html#gae3f6f638d84a380fd1399a5542d892e4", null ],
    [ "ODP_PKTIO_LINK_SPEED_56G", "group__odp__packet__io.html#gafa8d19d321ffcfb6f9b2730f99ad1c85", null ],
    [ "ODP_PKTIO_LINK_SPEED_100G", "group__odp__packet__io.html#ga6fd27595cd7c394fc29954d3a5548204", null ],
    [ "ODP_PKTIO_LINK_SPEED_200G", "group__odp__packet__io.html#ga36ce416fd9d9de32e5b6b23764fee0e1", null ],
    [ "ODP_PKTIO_LINK_SPEED_400G", "group__odp__packet__io.html#ga74a21c707a966b088019f767b6206bf6", null ],
    [ "odp_pktio_t", "group__odp__packet__io.html#gab2c9857b9be70ca49962a8f22d3cb657", null ],
    [ "odp_lso_profile_t", "group__odp__packet__io.html#ga98869cee05f72ca77e321cf44ade1e5a", null ],
    [ "odp_pktin_queue_t", "group__odp__packet__io.html#gaad3ad8a5368999d446f8225381f853e3", null ],
    [ "odp_pktout_queue_t", "group__odp__packet__io.html#gabd77b15e5fbbac12b8cef5742bddf821", null ],
    [ "odp_pktin_mode_t", "group__odp__packet__io.html#gacf078fb2dfcd425223082fb97936f031", null ],
    [ "odp_pktout_mode_t", "group__odp__packet__io.html#ga101aebe32b087d7bcfdc81d963ac2b77", null ],
    [ "odp_pktin_hash_proto_t", "group__odp__packet__io.html#ga8b2c055332aa85e75f7d5aef5e035d41", null ],
    [ "odp_pktio_op_mode_t", "group__odp__packet__io.html#gab872672191d8e5625ee779d8baab4204", null ],
    [ "odp_pktin_queue_param_ovr_t", "group__odp__packet__io.html#ga036f230c5eac0e0640b2a0b55572063c", null ],
    [ "odp_pktin_vector_config_t", "group__odp__packet__io.html#ga74f99a42bc580006b8644ceef2b050fd", null ],
    [ "odp_pktin_queue_param_t", "group__odp__packet__io.html#ga50ef290cb7613377d969158d21c5a997", null ],
    [ "odp_pktout_queue_param_t", "group__odp__packet__io.html#ga54fd05e2b4c628ea1ced37937a9c5cf0", null ],
    [ "odp_pktio_param_t", "group__odp__packet__io.html#gafdcafe53e807c06c7959a364157ed8a7", null ],
    [ "odp_pktin_config_opt_t", "group__odp__packet__io.html#ga1c7e5690f00e1d33759345a7fa2f067e", null ],
    [ "odp_pktout_config_opt_t", "group__odp__packet__io.html#ga116939e9e02b3ba6637eca193ce0d12a", null ],
    [ "odp_pktio_parser_layer_t", "group__odp__packet__io.html#ga32bc7c581aecbac1e45f4cb59bcac082", null ],
    [ "odp_pktio_parser_config_t", "group__odp__packet__io.html#ga74231a6528ee6eb7ca0d5974bc3a9686", null ],
    [ "odp_pktio_config_t", "group__odp__packet__io.html#ga58dd9772e9e20cb8fc62cea6b9c09355", null ],
    [ "odp_pktio_set_op_t", "group__odp__packet__io.html#gab8312a45ef3c0287e4c2aa6aca11b1ef", null ],
    [ "odp_lso_modify_t", "group__odp__packet__io.html#gaf4b212b96c3e6974ee5f300ce1292c4a", null ],
    [ "odp_lso_protocol_t", "group__odp__packet__io.html#gaff3eec78e6df8e4b5d6d97becbf30a02", null ],
    [ "odp_lso_capability_t", "group__odp__packet__io.html#ga6b421663f9755d921eca1347077ff07d", null ],
    [ "odp_pktin_vector_capability_t", "group__odp__packet__io.html#ga2593874219f3213d19045395e698578e", null ],
    [ "odp_pktio_capability_t", "group__odp__packet__io.html#ga62e1f031c5bbba2e0dc5c316071240d3", null ],
    [ "odp_lso_profile_param_t", "group__odp__packet__io.html#ga4e0b9bd71a74f857ae6a3771968ed3a8", null ],
    [ "odp_pktio_link_status_t", "group__odp__packet__io.html#ga4d5f63b818f23eacd81aaa713dff6343", null ],
    [ "odp_pktio_info_t", "group__odp__packet__io.html#ga5db4072057cbab7bd5075ccb45ab3fdd", null ],
    [ "odp_pktio_link_autoneg_t", "group__odp__packet__io.html#gad75d07ede0433bef0acba20a5fe6aeb3", null ],
    [ "odp_pktio_link_duplex_t", "group__odp__packet__io.html#ga994009b25f13fa661b42592d621e4ede", null ],
    [ "odp_pktio_link_pause_t", "group__odp__packet__io.html#ga5c856d5ace87e4e532a886c6e2eb720d", null ],
    [ "odp_pktio_link_info_t", "group__odp__packet__io.html#gab45d5f9704c5a947af51b28af210e96f", null ],
    [ "odp_pktio_stats_t", "group__odp__packet__io.html#ga327c35f2ef419bf8944c6f5b6816c2e3", null ],
    [ "odp_pktin_mode_t", "group__odp__packet__io.html#gaf5d539df6fc8af55d96357c784a54b8f", [
      [ "ODP_PKTIN_MODE_DIRECT", "group__odp__packet__io.html#ggaf5d539df6fc8af55d96357c784a54b8fa36f229784bc7dcef48203b02b3938357", null ],
      [ "ODP_PKTIN_MODE_SCHED", "group__odp__packet__io.html#ggaf5d539df6fc8af55d96357c784a54b8faa4f62db7acbe492a3b80f3f782a04dd5", null ],
      [ "ODP_PKTIN_MODE_QUEUE", "group__odp__packet__io.html#ggaf5d539df6fc8af55d96357c784a54b8fa502928549b130729ee8c71ab14cee89a", null ],
      [ "ODP_PKTIN_MODE_DISABLED", "group__odp__packet__io.html#ggaf5d539df6fc8af55d96357c784a54b8fa69fe9800b338a86abc1141fcf89a67f2", null ]
    ] ],
    [ "odp_pktout_mode_t", "group__odp__packet__io.html#ga4e0c26e40b1166b07142302d12e8467d", [
      [ "ODP_PKTOUT_MODE_DIRECT", "group__odp__packet__io.html#gga4e0c26e40b1166b07142302d12e8467da9e6184e8d605ce199cfa54d6c50153ff", null ],
      [ "ODP_PKTOUT_MODE_QUEUE", "group__odp__packet__io.html#gga4e0c26e40b1166b07142302d12e8467dae90395ccf6ce924a3a6cb375ca42f279", null ],
      [ "ODP_PKTOUT_MODE_TM", "group__odp__packet__io.html#gga4e0c26e40b1166b07142302d12e8467daa92b70f23f0312287463f414c6340c23", null ],
      [ "ODP_PKTOUT_MODE_DISABLED", "group__odp__packet__io.html#gga4e0c26e40b1166b07142302d12e8467daf34ccc9cab59f9ced90942c7810a149a", null ]
    ] ],
    [ "odp_pktio_op_mode_t", "group__odp__packet__io.html#gacf2c32e76d9d0eef9106b5fdd68cedc8", [
      [ "ODP_PKTIO_OP_MT", "group__odp__packet__io.html#ggacf2c32e76d9d0eef9106b5fdd68cedc8af3637232af55eb954ba439b2d33120d3", null ],
      [ "ODP_PKTIO_OP_MT_UNSAFE", "group__odp__packet__io.html#ggacf2c32e76d9d0eef9106b5fdd68cedc8a1e3d825b302ee6213937172a6b54b140", null ]
    ] ],
    [ "odp_lso_modify_t", "group__odp__packet__io.html#ga77fb85dc5ee288b5259e0cc1ad3ae4b1", [
      [ "ODP_LSO_ADD_SEGMENT_NUM", "group__odp__packet__io.html#gga77fb85dc5ee288b5259e0cc1ad3ae4b1a0f1e88d6a38e888df2c8b1bace54eb26", null ],
      [ "ODP_LSO_ADD_PAYLOAD_LEN", "group__odp__packet__io.html#gga77fb85dc5ee288b5259e0cc1ad3ae4b1adba3b5d55237ccf02309c0befeec40c3", null ],
      [ "ODP_LSO_ADD_PAYLOAD_OFFSET", "group__odp__packet__io.html#gga77fb85dc5ee288b5259e0cc1ad3ae4b1ab81b636db9667c7b140d9961aac52d6a", null ]
    ] ],
    [ "odp_lso_protocol_t", "group__odp__packet__io.html#ga0e4ebed445315421ce5c337783b9629e", [
      [ "ODP_LSO_PROTO_NONE", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629ea0d0c327f554342373c3d8215e960046c", null ],
      [ "ODP_LSO_PROTO_CUSTOM", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629eaa0be15f2b3303f12dfdb4069db33531a", null ],
      [ "ODP_LSO_PROTO_IPV4", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629ea02f5567ef1a138475c7dad2c0a9c15d5", null ],
      [ "ODP_LSO_PROTO_IPV6", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629ea377b2ff7cf62b2052f1f140830e7e2b2", null ],
      [ "ODP_LSO_PROTO_TCP_IPV4", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629ea4632e915762b4bc54914d904bc10f606", null ],
      [ "ODP_LSO_PROTO_TCP_IPV6", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629ead7ebf3e21914bddc9ab5e11bd6bdf883", null ],
      [ "ODP_LSO_PROTO_SCTP_IPV4", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629eaabbd33aa79ae6a7c408d6dd88a6b89db", null ],
      [ "ODP_LSO_PROTO_SCTP_IPV6", "group__odp__packet__io.html#gga0e4ebed445315421ce5c337783b9629eaeab60aa9dbbcf5498c3b738dcd33b5fa", null ]
    ] ],
    [ "odp_pktio_link_status_t", "group__odp__packet__io.html#gaf98709ceb48d0fd3620b6788467ee5f7", null ],
    [ "odp_pktio_link_autoneg_t", "group__odp__packet__io.html#ga7179cf6723255158d1f7dfefe27f45dd", [
      [ "ODP_PKTIO_LINK_AUTONEG_UNKNOWN", "group__odp__packet__io.html#gga7179cf6723255158d1f7dfefe27f45dda1afbe9de3a16a9210a2ea7cb02e638a5", null ],
      [ "ODP_PKTIO_LINK_AUTONEG_OFF", "group__odp__packet__io.html#gga7179cf6723255158d1f7dfefe27f45ddabec25760c5a30899e83c43071ebac19a", null ],
      [ "ODP_PKTIO_LINK_AUTONEG_ON", "group__odp__packet__io.html#gga7179cf6723255158d1f7dfefe27f45ddaef9b227c6ac1426fbf050e35aea11385", null ]
    ] ],
    [ "odp_pktio_link_duplex_t", "group__odp__packet__io.html#ga62dd45bcf8ac13a39df24d1d23bf86fa", null ],
    [ "odp_pktio_link_pause_t", "group__odp__packet__io.html#gae96ada663d380fe4ee3d9c5ab1fd57e6", null ],
    [ "odp_pktio_open", "group__odp__packet__io.html#ga7ae4f96ab09d468fff3623d90db306f9", null ],
    [ "odp_pktio_capability", "group__odp__packet__io.html#gaba80778db7ec138977ed9e473bd1faaa", null ],
    [ "odp_pktio_max_index", "group__odp__packet__io.html#ga0be6e0bf709786c2c7eb5ae78c5a3246", null ],
    [ "odp_pktio_config", "group__odp__packet__io.html#ga7f9faaf250d3d0d87478ff9f13680bf1", null ],
    [ "odp_pktin_queue_config", "group__odp__packet__io.html#gaf9dce6b2cc13ad715e777416dce88194", null ],
    [ "odp_pktout_queue_config", "group__odp__packet__io.html#gafc2120185a8da74540feade78623f3b3", null ],
    [ "odp_pktin_event_queue", "group__odp__packet__io.html#ga7a0b3f09fd55576daa9617dc29c60ace", null ],
    [ "odp_pktin_queue", "group__odp__packet__io.html#gaa9d5cad745c9744ce026412ccf33d019", null ],
    [ "odp_pktout_event_queue", "group__odp__packet__io.html#gab2089a43d1901927c000bc43af3b6a2b", null ],
    [ "odp_pktout_queue", "group__odp__packet__io.html#ga41a3ebb7ac489b85a5f7560e4d4f77f5", null ],
    [ "odp_pktio_start", "group__odp__packet__io.html#ga9a5746e73616297fabb8d4caaa6d0333", null ],
    [ "odp_pktio_stop", "group__odp__packet__io.html#gab34cb9679bf99d1b3366f06a9b155c41", null ],
    [ "odp_pktio_close", "group__odp__packet__io.html#ga2e481c9f2f073cc337aff7f4c65cba7c", null ],
    [ "odp_pktio_lookup", "group__odp__packet__io.html#ga3109661131ac13ab3b65ea843bb59a77", null ],
    [ "odp_pktin_recv", "group__odp__packet__io.html#gab3dd2331f6ba05e074333fd5badae340", null ],
    [ "odp_pktin_recv_tmo", "group__odp__packet__io.html#gad9029d423085d5b963a9a45b3964475d", null ],
    [ "odp_pktin_recv_mq_tmo", "group__odp__packet__io.html#gac34003530589011a1f1616cb7bcc98ae", null ],
    [ "odp_pktin_wait_time", "group__odp__packet__io.html#gab128d70590d0dc1ded0c83908dac8394", null ],
    [ "odp_pktout_send", "group__odp__packet__io.html#gacb5db3a908775522e316c8b8ccb36498", null ],
    [ "odp_lso_profile_param_init", "group__odp__packet__io.html#ga3dff74e3f4b23eb30098febce1808cc4", null ],
    [ "odp_lso_profile_create", "group__odp__packet__io.html#ga178c2165219774dd8b37e3713e89170d", null ],
    [ "odp_lso_profile_destroy", "group__odp__packet__io.html#ga602389ee38439054717acd74bbba38e3", null ],
    [ "odp_pktout_send_lso", "group__odp__packet__io.html#ga7d3f511443262e8a0dfab1475de124b3", null ],
    [ "odp_pktio_mtu", "group__odp__packet__io.html#gadcfbbd3abfbaaff15a22d2953161f0a7", null ],
    [ "odp_pktio_promisc_mode_set", "group__odp__packet__io.html#ga506ab30c6737e9fbd815f0044396893b", null ],
    [ "odp_pktio_promisc_mode", "group__odp__packet__io.html#ga2c232afd3b0c7afd7e9685a0957b84bd", null ],
    [ "odp_pktin_maxlen", "group__odp__packet__io.html#ga2efe22ecde5d8c72a27ff213451e4fe3", null ],
    [ "odp_pktout_maxlen", "group__odp__packet__io.html#ga3373b50fe0531f6c58f25d51972f414e", null ],
    [ "odp_pktio_maxlen_set", "group__odp__packet__io.html#ga4efd726f993e7fe99d44f04f5ebe2141", null ],
    [ "odp_pktio_mac_addr", "group__odp__packet__io.html#ga04d67c951aff803cf718940e0783ef8f", null ],
    [ "odp_pktio_mac_addr_set", "group__odp__packet__io.html#ga986da72a3a65ea6ac754bb80d8b32189", null ],
    [ "odp_pktio_default_cos_set", "group__odp__packet__io.html#gaea05ed30a85e460135f0a28589fc0f19", null ],
    [ "odp_pktio_error_cos_set", "group__odp__packet__io.html#ga35e5e97f35eca5252f8fa757ad1dc85c", null ],
    [ "odp_pktio_skip_set", "group__odp__packet__io.html#gaff2e7490295f1348734aefefdc550225", null ],
    [ "odp_pktio_headroom_set", "group__odp__packet__io.html#ga15fa0aa199a27357a9afd83dc424529f", null ],
    [ "odp_pktio_index", "group__odp__packet__io.html#gab552d0e69245ade3de47130c2b9c5203", null ],
    [ "odp_pktio_to_u64", "group__odp__packet__io.html#gadb1dbe3dd66d31e15ec3d3e53880a446", null ],
    [ "odp_pktio_param_init", "group__odp__packet__io.html#ga26d165a22d659770eba0c48543860881", null ],
    [ "odp_pktin_queue_param_init", "group__odp__packet__io.html#ga055eabf5622e6745afbe9a1550ad1d78", null ],
    [ "odp_pktout_queue_param_init", "group__odp__packet__io.html#gab09475da05438a53b2d0fca96aab7515", null ],
    [ "odp_pktio_config_init", "group__odp__packet__io.html#ga6a1c262ef999e8a73ac107c182b67d33", null ],
    [ "odp_pktio_print", "group__odp__packet__io.html#ga8fdd705d18d4aeb81528e68c0294e984", null ],
    [ "odp_pktio_link_status", "group__odp__packet__io.html#gacb8314d65b914add634b57d50939a296", null ],
    [ "odp_pktio_info", "group__odp__packet__io.html#ga3f703418ffbd054ce430a7f0f23be0f8", null ],
    [ "odp_pktio_link_info", "group__odp__packet__io.html#ga2f40d549734ab0217e7accd94ad36def", null ],
    [ "odp_pktin_ts_res", "group__odp__packet__io.html#ga378edc104a08057579124cc5465760d2", null ],
    [ "odp_pktin_ts_from_ns", "group__odp__packet__io.html#ga0fdbbab7f934009a1c2243fad5b0adf6", null ],
    [ "odp_pktio_ts_res", "group__odp__packet__io.html#ga1f75fba52ac4d74a2a73a678b33eb6eb", null ],
    [ "odp_pktio_ts_from_ns", "group__odp__packet__io.html#ga4043430ef50ba1883d6e3d9da9213f6f", null ],
    [ "odp_pktio_time", "group__odp__packet__io.html#gaaf1982363c29afa19d8e2acdb10163d9", null ],
    [ "odp_pktout_ts_read", "group__odp__packet__io.html#ga49733cf1cfdaa6360292395f1f94e7ae", null ],
    [ "odp_pktio_stats", "group__odp__packet__io.html#gadda58d08490fec62aa44ad8b237d3fed", null ],
    [ "odp_pktio_stats_reset", "group__odp__packet__io.html#gaf2681948d7a37a9bd4cc26db8e47d997", null ]
];