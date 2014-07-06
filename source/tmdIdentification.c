#include "tmdIdentification.h"

iosHashes iosHash[base_number] = {
// {hash, hash, hash, hash, hash}, Base, InfoText},
{{0x20e60607, 0x4e02c484, 0x2bbc5758, 0xee2b40fc, 0x35a68b0a}, 38, "rev 13a"  },		// cIOSrev13a
{{0x620c57c7, 0xd155b67f, 0xa451e2ba, 0xfb5534d7, 0xaa457878}, 38, "rev 13b"  },		// cIOSrev13b
{{0x3c968e54, 0x9e915458, 0x9ecc3bda, 0x16d0a0d4, 0x8cac7917}, 37, "rev 18"   },		// cIOS37 rev18
{{0xe811bca8, 0xe1df1e93, 0x779c40e6, 0x2006e807, 0xd4403b97}, 38, "rev 18"   },		// cIOS38 rev18
{{0x697676f0, 0x7a133b19, 0x881f512f, 0x2017b349, 0x6243c037}, 57, "rev 18"   },		// cIOS57 rev18
{{0x34ec540b, 0xd1fb5a5e, 0x4ae7f069, 0xd0a39b9a, 0xb1a1445f}, 60, "rev 18"   },		// cIOS60 rev18
{{0xd98a4dd9, 0xff426ddb, 0x1afebc55, 0x30f75489, 0x40b27ade}, 70, "rev 18"   },		// cIOS70 rev18
{{0x0a49cd80, 0x6f8f87ff, 0xac9a10aa, 0xefec9c1d, 0x676965b9}, 37, "rev 19"   },		// cIOS37 rev19
{{0x09179764, 0xeecf7f2e, 0x7631e504, 0x13b4b7aa, 0xca5fc1ab}, 38, "rev 19"   },		// cIOS38 rev19
{{0x6010d5cf, 0x396415b7, 0x3c3915e9, 0x83ded6e3, 0x8f418d54}, 57, "rev 19"   },		// cIOS57 rev19
{{0x589d6c4f, 0x6bcbd80a, 0xe768f258, 0xc53a322c, 0xd143f8cd}, 60, "rev 19"   },		// cIOS60 rev19
{{0x8969e0bf, 0x7f9b2391, 0x31ecfd88, 0x1c6f76eb, 0xf9418fe6}, 70, "rev 19"   },		// cIOS70 rev19
{{0x30aeadfe, 0x8b6ea668, 0x446578c7, 0x91f0832e, 0xb33c08ac}, 36, "rev 20"   },		// cIOS36 rev20
{{0xba0461a2, 0xaa26eed0, 0x482c1a7a, 0x59a97d94, 0xa607773e}, 37, "rev 20"   },		// cIOS37 rev20
{{0xb694a33e, 0xf5040583, 0x0d540460, 0x2a450f3c, 0x69a68148}, 38, "rev 20"   },		// cIOS38 rev20
{{0xf6058710, 0xfe78a2d8, 0x44e6397f, 0x14a61501, 0x66c352cf}, 53, "rev 20"   },		// cIOS53 rev20
{{0xfa07fb10, 0x52ffb607, 0xcf1fc572, 0xf94ce42e, 0xa2f5b523}, 55, "rev 20"   },		// cIOS55 rev20
{{0xe30acf09, 0xbcc32544, 0x490aec18, 0xc276cee6, 0x5e5f6bab}, 56, "rev 20"   },		// cIOS56 rev20
{{0x595ef1a3, 0x57d0cd99, 0x21b6bf6b, 0x432f6342, 0x605ae60d}, 57, "rev 20"   },		// cIOS57 rev20
{{0x687a2698, 0x3efe5a08, 0xc01f6ae3, 0x3d8a1637, 0xadab6d48}, 60, "rev 20"   },		// cIOS60 rev20
{{0xea6610e4, 0xa6beae66, 0x887be72d, 0x5da3415b, 0xa470523c}, 61, "rev 20"   },		// cIOS61 rev20
{{0x64e1af0e, 0xf7167fd7, 0x0c696306, 0xa2035b2d, 0x6047c736}, 70, "rev 20"   },		// cIOS70 rev20
{{0x0df93ca9, 0x833cf61f, 0xb3b79277, 0xf4c93cd2, 0xcd8eae17}, 80, "rev 20"   },		// cIOS80 rev20
{{0x06c7a8e2, 0x3485e466, 0xd9ba56b6, 0x27d1cf5f, 0x4d8e451c}, 38, "rev 18"   },		// cIOS38 rev18+
{{0x1ec125a0, 0xa04a6f60, 0xbd6f6faf, 0xbcb13d62, 0xe2a757b7}, 57, "rev 18"   },		// cIOS57 rev18+
{{0xdf89b1b8, 0x340a001c, 0x032bdf54, 0x8679b8cb, 0x1bfb3490}, 57, "rev 18"   },    	// cIOS57 rev18+
{{0x2fd88e4b, 0xd47fa8f1, 0x1bd39d7e, 0x79f88f9d, 0xd1ab19d8}, 36, "rev 3351" },    	// (c)IOS36 (IOS236)
{{0x074dfb39, 0x90a5da61, 0x67488616, 0x68ccb747, 0x3a5b59b3}, 36, "rev 21"   },	    // IOS249 r21 Base: 36
{{0x6956a016, 0x59542728, 0x8d2efade, 0xad8ed01e, 0xe7f9a780}, 37, "rev 21"   },	    // IOS249 r21 Base: 37
{{0xdc8b23e6, 0x9d95fefe, 0xac10668a, 0x6891a729, 0x2bdfbca0}, 38, "rev 21"   }, 		// IOS249 r21 Base: 38
{{0xaa2cdd40, 0xd628bc2e, 0x96335184, 0x1b51404c, 0x6592b992}, 53, "rev 21"   }, 	  	// IOS249 r21 Base: 53
{{0x4a3d6d15, 0x014f5216, 0x84d65ffe, 0x6daa0114, 0x973231cf}, 55, "rev 21"   },		// IOS249 r21 Base: 55
{{0xca883eb0, 0x3fe8e45c, 0x97cc140c, 0x2e2d7533, 0x5b369ba5}, 56, "rev 21"   }, 		// IOS249 r21 Base: 56
{{0x469831dc, 0x918acc3e, 0x81b58a9a, 0x4493dc2c, 0xaa5e57a0}, 57, "rev 21"   }, 		// IOS249 r21 Base: 57
{{0xe5af138b, 0x029201c7, 0x0c1241e7, 0x9d6a5d43, 0x37a1456a}, 58, "rev 21"   }, 		// IOS249 r21 Base: 58
{{0x0fdee208, 0xf1d031d3, 0x6fedb797, 0xede8d534, 0xd3b77838}, 60, "rev 21"   }, 		// IOS249 r21 Base: 60
{{0xaf588570, 0x13955a32, 0x001296aa, 0x5f30e37f, 0x0be91316}, 61, "rev 21"   }, 		// IOS249 r21 Base: 61
{{0x50deaba2, 0x9328755c, 0x7c2deac8, 0x385ecb49, 0x65ea3b2b}, 70, "rev 21"   }, 		// IOS249 r21 Base: 70
{{0x811b6a0b, 0xe26b9419, 0x7ffd4930, 0xdccd6ed3, 0x6ea2cdd2}, 80, "rev 21"   },  		// IOS249 r21 Base: 80
{{0xe40e13e0, 0xf0444cc0, 0x4fb40daa, 0x6487c3d8, 0x488879a2}, 38, "rev 17"   },	 	// cIOS38 r17
{{0xabcdd440, 0x24ed5dc1, 0x7b8c1799, 0xf8904659, 0x998cb370}, 38, "d2x-v2"   },		// d2x-v2 r21 Base: 38 
{{0x841743a4, 0xf75ce640, 0xb91b2d6a, 0xccae31dd, 0xf5a507d7}, 57, "d2x-v2"   },		// d2x-v2 r21 Base: 57 
{{0x80fb2483, 0x5357a521, 0xf9013063, 0xd1a60db6, 0xd8960d6c}, 56, "d2x-v2"   },		// d2x-v2 r21 Base: 56
{{0xac5e096d, 0x9715752b, 0x1607d4c1, 0x70380e84, 0xc3684eae}, 37, "d2x-v2"   },		// d2x-v2 r21 Base: 37
{{0xc9362096, 0xd39ffadd, 0x3395912d, 0x06f21da5, 0x4aed0679}, 58, "rev 21"   },		// cIOS58 r21
{{0x60345733, 0xdf1bf9a4, 0x20a72585, 0xea86cf66, 0x12b7df6a}, 57, "rev 21"   },		// cIOS57 r21
{{0x5f178a98, 0x4c4486ee, 0xcdb51ca7, 0xb0b172a0, 0x91ab0d2b}, 38, "rev 21"   },		// cIOS38 r21
{{0x115a5bc1, 0x28fd5a1f, 0xc5ca4b76, 0xd54d4016, 0x5f93e406}, 56, "rev 21"   },		// cIOS56 r21
{{0xd3198a49, 0xd4be48b6, 0x74c50488, 0xdf870e48, 0x91ed14a9}, 57, "rev 21"   },		// cIOS57 r21
{{0x3f0fa343, 0x68413800, 0xe31729a6, 0x1805e71e, 0x680f9add}, 37, "d2xv3"    },		// d2x-v3 r21 Base: 37 	
{{0xec25c714, 0xf63e6696, 0x10d6da7a, 0x4c331c64, 0xc3168ce6}, 38, "d2x-v3"   },		// d2x-v3 r21 Base: 38 	
{{0xce0b9786, 0x6720fa8a, 0x4c09b54e, 0x42982999, 0xd447208b}, 56, "d2x-v3"   },		// d2x-v3 r21 Base: 56 	
{{0xa24edf73, 0x572c5853, 0xcfe01a7e, 0x050afbf8, 0x6baa0d62}, 57, "d2x-v3"   },	    // d2x-v3 r21 Base: 57 	
{{0x133c7710, 0x26d289de, 0x4622a841, 0xd32975d3, 0x35f87e31}, 58, "d2x-v3"   },		// d2x-v3 r21 Base: 58 	
{{0x65944f33, 0x11f578c4, 0x3fdb6015, 0x084a307f, 0xe7040d61}, 37, "d2x-v1"   },		// d2x-v1 r21 Base: 37 	
{{0x8371e0d3, 0xde2839d3, 0x0a116be3, 0xb8bdc04a, 0x6ed83a4e}, 38, "d2x-v1"   },	    // d2x-v1 r21 Base: 38 	
{{0x38f9a0d4, 0x367e0808, 0xe2fff71b, 0x1263221a, 0x60bc29cb}, 56, "d2x-v1"   },		// d2x-v1 r21 Base: 56 	
{{0x09d79d2a, 0x73e3c439, 0x1c0b9e3b, 0x7909b59d, 0xfdd77f74}, 57, "d2x-v1"   },		// d2x-v1 r21 Base: 57
{{0x20ee9790, 0x22cf3b0a, 0x402c4620, 0x129ad758, 0x6c94386b}, 37, "d2x-v1"   },		// d2x-v1 r65535 Base: 37 	
{{0xc2b448f4, 0x9537e0d7, 0x5730372a, 0x0eef2790, 0x4096bc2b}, 38, "d2x-v1"   },		// d2x-v1 r65535 Base: 38 	
{{0xd6a2a8cd, 0x0b3c65df, 0x34791948, 0x04c6bbb8, 0xbd58fb77}, 56, "d2x-v1"   },		// d2x-v1 r65535 Base: 56 	
{{0x9d8c98fc, 0x189b531b, 0xdfff0981, 0xe3f78f60, 0xa004a6fc}, 57, "d2x-v1"   },		// d2x-v1 r65535 Base: 57 
{{0x9daa8020, 0xf65bf973, 0x0b0ae31e, 0xcfcbc83f, 0x708ba2cc}, 37, "d2x-v2"   },		// d2x-v2 r65535 Base: 37 	
{{0x652b4055, 0xa57b8777, 0x217fc97e, 0x1dacd460, 0xd8795f57}, 38, "d2x-v2"   },		// d2x-v2 r65535 Base: 38 	
{{0x68fa7d61, 0x30ffd971, 0x83e1a056, 0x7e3c693b, 0xa18465ed}, 56, "d2x-v2"   },		// d2x-v2 r65535 Base: 56 	
{{0xb3437bbf, 0x832f21d8, 0x8bdad236, 0x98fdd32e, 0xbaff6e2c}, 57, "d2x-v2"   },		// d2x-v2 r21 Base: 57 
{{0xc549172f, 0x13cd642a, 0xc09ab7f7, 0xbfa36d8b, 0x56e3c2d7}, 37, "d2x-v3"   },		// d2x-v3 r65535 Base: 37 	
{{0x8b64539a, 0x8f253b81, 0x02408620, 0x7377b5f9, 0xdbd70b6b}, 38, "d2x-v3"   },	    // d2x-v3 r65535 Base: 38 	
{{0xa923319c, 0xbb6d25a8, 0xae8eab82, 0xa0376dc6, 0x11302999}, 56, "d2x-v3"   },		// d2x-v3 r65535 Base: 56 	
{{0xda1ff450, 0xe2710584, 0x44411c4d, 0xb7486df5, 0xfbcd67d7}, 57, "d2x-v3"   },	    // d2x-v3 r65535 Base: 57 	
{{0x2d24fbca, 0xb8c075b3, 0x921f3b58, 0x8bcf0cf6, 0x5568cd1e}, 58, "d2x-v3"   },		// d2x-v3 r65535 Base: 58 
{{0x2bd8749e, 0xa0752f26, 0x480ba81e, 0xe2e90ece, 0x1b515c9d}, 36, "rev 3351" },		// (c)IOS36 (IOS236)
{{0xff6154a5, 0xa368f31e, 0x99a685af, 0x9b19b571, 0x28badb20}, 37, "d2x-v3"   },		// d2x-v3 r65535 Base: 37
{{0xa923319c, 0xbb6d25a8, 0xae8eab82, 0xa0376dc6, 0x11302999}, 56, "d2x-v3"   },		// d2x-v3 r65535 Base: 56
{{0x5c7be813, 0xd814c8b2, 0x6f8785f4, 0xa964e276, 0x5a0fa6ce}, 38, "d2x-v3"   },		// d2x-v3 r65535 Base: 38
{{0xe32539ce, 0x43acc96c, 0x9b27dd23, 0x53b04172, 0x6f303a5c}, 57, "d2x-v3"   },		// d2x-v3 r65535 Base: 57
{{0xaa0688eb, 0x328e516d, 0x89bef8f7, 0xd1151bd8, 0x81af4db8}, 58, "d2x-v3"   },		// d2x-v3 r65535 Base: 58
{{0x6a8376ab, 0xa466f9b6, 0x1403dfcf, 0x120fbe9e, 0x48e3acbe}, 80, "rev 21"   },		// cIOS80 r21 
{{0x4db1be1e, 0xdb4d0c1c, 0x75900233, 0xeeae3fc6, 0x9bb86d3e}, 37, "d2x-v3"   },		// d2x-v3 r65535 Base: 37
{{0x86f60709, 0x2e580cba, 0x922cbed8, 0x28d053bd, 0xff378145}, 56, "d2x-v3"   },		// d2x-v3 r65535 Base: 56
{{0x7a7a0094, 0x2991ea1c, 0xaff02047, 0x6d1ffffc, 0x0c9e53a6}, 58, "d2x-v3"   },		// d2x-v3 r65535 Base: 58
{{0x0facacb9, 0x34c2886c, 0x5b13b098, 0x2ae699e9, 0xec47d8ee}, 57, "d2x-v3"   },		// d2x-v3 r65535 Base: 57
{{0x90badb7b, 0xeef5196e, 0x5f32f59b, 0x29a98f2d, 0xc0e64260}, 38, "d2x-v3"   },		// d2x-v3 r65535 Base: 38
{{0x57671632, 0x131c0441, 0x78991fe3, 0x7d8b1baa, 0x99f597bf}, 38, "rev 17b"  },		// cIOS38 rev17b
{{0x5db466c0, 0xd04ada02, 0x4348dbca, 0x74729d0c, 0x3cb2c30c}, 80, "rev 6944" },		// IOS80 rev 6944	
{{0x0312f927, 0xabcc84db, 0x319bf5f9, 0xa7b5432c, 0x66b5fa47}, 30, "rev 2576" },		// IOS30 rev 2576
{{0xac131d4c, 0xfa6e61a0, 0x170cc7c4, 0xa7cbbbc7, 0xa50c7c09}, 50, "rev 4889" },		// IOS50 rev 4889
{{0xd18fed7b, 0x26b5bf56, 0xe59e0077, 0xa7d34fa4, 0xe53e5040}, 51, "rev 4633" },		// IOS51 rev 4633
{{0xbe46b9ef, 0x00bc99f0, 0x8d40dc62, 0x5a1af1d9, 0xa9ad2960}, 60, "rev 6174" },		// IOS60 rev 6174
{{0xc65c0e7b, 0xaa023c9a, 0x08e31648, 0x36d105ff, 0x37c490d4}, 70, "rev 6687" },		// IOS70 rev 6687
{{0xc2e6bdf7, 0xdddfa3aa, 0x738c0637, 0xfd21ed87, 0xac4487df}, 11, "rev 10"   },		// IOS11 rev 10
{{0xf6ce3ce6, 0xb581c754, 0xa100b63e, 0xe59ea2ca, 0x8d4a53a9}, 20, "rev 12"   },		// IOS70 rev 12
{{0x9957b893, 0x158b1c82, 0xc0e2ee3c, 0xf024697b, 0x70349fd2}, 58, "d2x-v5a1" },		// d3x-v5alpha1 r65535 Base: 58
{{0x666b6be7, 0x7ce68420, 0xe5390361, 0xd7b7fef3, 0xfab6fbb8}, 57, "d2x-v5a1" },		// d2x-v5alpha1 r65535 Base: 57
{{0x0e5e820f, 0x7f7a995f, 0xfeadfb17, 0xa951b3f3, 0xc37df8f3}, 38, "d2x-v5a1" },		// d2x-v5alpha1 r65535 Base: 38
{{0x1ffeb8d3, 0x73960032, 0xce2c7e7a, 0xb0edb2af, 0xa2edcaf6}, 56, "d2x-v5a1" },		// d2x-v5alpha1 r65535 Base: 56
{{0xa6d6b431, 0xa17d4300, 0xb0545865, 0x178ba8ad, 0x9db775a8}, 37, "d2x-v5a1" },		// d2x-v5alpha1 r65535 Base: 37
{{0x285bffe0, 0x423f5a42, 0x1245eb42, 0xbaf6a560, 0x24692cd6}, 38, "rev 20"   },		// cIOS38 rev20
{{0xe9341b4d, 0x9e17a081, 0xae283e2c, 0x32cd9787, 0xe94731d2}, 38, "rev 20"   },		// cIOS38 rev20
{{0x33123c8e, 0xe7c4092c, 0x3b373cd2, 0x5398e557, 0xa5083896}, 56, "rev 20"   },		// cIOS56 rev20
{{0x62d422f1, 0x5d5634a2, 0xf558fd66, 0x2e5ade12, 0xea308923}, 56, "rev 20"   },		// cIOS56 rev20
{{0xebb81264, 0xc41331bc, 0x7fe76c17, 0xcf400d15, 0x25035bec}, 57, "rev 20"   },		// cIOS57 rev20
{{0x7dd6a102, 0x91413451, 0xb3b460bc, 0xbf6dff97, 0x79407afa}, 57, "rev 20"   },		// cIOS57 rev20
{{0x651a74de, 0x7e624b0c, 0x15e582c5, 0x94ced058, 0x27a9e4b2}, 38, "rev 21"   },		// cIOS38 rev21
{{0x1e8e63dd, 0x897a4e34, 0x79aa9395, 0x6bd8167e, 0x515101ad}, 38, "rev 21"   },		// cIOS38 rev21
{{0x018699f6, 0x1886ee6c, 0xb4144505, 0x9a5ba565, 0x7f1659be}, 56, "rev 21"   },		// cIOS56 rev21
{{0x3876b3c9, 0x4d8db267, 0xcaa0c752, 0x63fb38f8, 0x2a0226a5}, 56, "rev 21"   },		// cIOS56 rev21
{{0x1b4fb650, 0x3aaf1665, 0xebfbc1cf, 0xfe52c3ad, 0x711989f0}, 57, "rev 21"   },		// cIOS57 rev21
{{0x57efc1dc, 0x02806212, 0x17239dee, 0x31c01423, 0x14340f3f}, 57, "rev 21"   },		// cIOS57 rev21
{{0xed1afef3, 0x37a084d7, 0xa7f84e81, 0xcaba017a, 0x7707963d}, 38, "d2x-v4"   },		// d2x cIOS38 v4
{{0xc79f7915, 0x0f3c7848, 0x0fd4dafb, 0x62b61ab8, 0x675d463f}, 38, "d2x-v4"   },		// d2x cIOS38 v4
{{0x5167c1fa, 0xcf54edcd, 0x6d688d62, 0x0387c1a7, 0xbcf109e6}, 56, "d2x-v4"   },		// d2x cIOS56 v4
{{0x697f935f, 0x25929794, 0x5dd16218, 0xa15dedc9, 0x0fe63ead}, 56, "d2x-v4"   },		// d2x cIOS56 v4
{{0x385567e3, 0xeaf6d36a, 0x874b1a03, 0xabc5e3d6, 0xa74da383}, 57, "d2x-v4"   },		// d2x cIOS57 v4
{{0x3110d72d, 0x96a9aa14, 0xa5d35afe, 0xb991f1e4, 0xc88a804b}, 57, "d2x-v4"   },		// d2x cIOS57 v4
{{0xfce5cb4d, 0x055ce1ab, 0xaba3a4b1, 0x7b298665, 0x0cafbcc6}, 80, "rev 6944" },		// IOS80 rev 6944
{{0x95e2193d, 0x4e80e7ea, 0xb2ed6b5e, 0x8012d483, 0x129dc921}, 70, "rev 6687" },		// IOS70 rev 6687
{{0xb7268554, 0x8a7194c4, 0x2a1d6039, 0x2271ace5, 0x62db98b9}, 60, "rev 6174" },		// IOS60 rev 6174
{{0x1a3200ec, 0x7eaa20de, 0x69c02000, 0x496f2d01, 0x506a7824}, 50, "rev 4889" },		// IOS50 rev 4889
{{0xea390bf6, 0x4b1f6640, 0xdb46f605, 0xb3c1f6fc, 0x62f173b9}, 30, "rev 2576" },		// IOS30 rev 2576
{{0x97f4ad1a, 0x437842a8, 0x015435c7, 0xa2b25178, 0x3564265d}, 20, "rev 12"   },		// IOS20 rev 212
{{0xe42a9e18, 0xa967eb9c, 0x94769498, 0x917c9ab9, 0x6169e7ca}, 11, "rev 10"   }			// IOS11 rev 10
};