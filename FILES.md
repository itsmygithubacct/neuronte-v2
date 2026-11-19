# The shipped files

124 files, 144,108,937 bytes. Every file is a file of Neuronte, copied byte for byte under the same name: the 58 files marked 2.6.0 (the 48 faces of Neuronte Sans Metric and the five wide bundles with their lists) are files of Neuronte 0.1.7, as pushed on its branches `work/neuronte-metric-sans` at [0aa0a4c](https://github.com/itsmygithubacct/neuronte/tree/v0.1.7) and `work/neuronte-wide` at [985c177](https://github.com/itsmygithubacct/neuronte/tree/v0.1.7), each checked against 0.1.7's `weights/SHA256SUMS`; the 33 desktop faces marked 2.5.0 are files of Neuronte at commit [bfd0a2d](https://github.com/itsmygithubacct/neuronte/tree/bfd0a2d066161eaa5abeae9747e151ff345702ef) (branch `work/neuronte-latin1-faces`; no Neuronte tag holds them yet), and every other file is a file of [Neuronte 0.1.6](https://github.com/itsmygithubacct/neuronte/tree/v0.1.6). Each 2.5.0 face is the file it replaces (the ASCII-only `-v1` face of the same size, `neuronte-ui-scaled-latin1-v0` with its italic advances `-v0`, and `neuronte-ui-large-latin1-v1`) with nets for the rest of Latin-1 added beside its unchanged nets. Of the others, all but the eighteen marked 2.4.0 are also files of 0.1.5, and all but those and the eight marked 2.3.0 are files of 0.1.4. The four bundles marked 2.3.0 are opened beside the terminal bundle, each with the list of the characters it was trained on; so are the six terminal bundles marked 2.4.0. The three bar bundles marked 2.4.0 are for a terminal's own bars. The three desktop files for 17 px and up do the work of 33 files of 0.1.2, as the three of 2.1.1 they replace did, and the terminal bundle marked 2.2.0 takes the place of `neuronte-split-v8.nrt`. The last column is the file of 0.1.1 that did the same job.

| Job | File | Bytes | SHA-256 | In 0.1.1 |
|---|---|---:|---|---|
| Terminal font, every character (2.2.0) | `neuronte-split-v9.nrt` | 3,607,088 | `0a92f223d47f1680929b2c5d7512d1a1728d4871e6653b33cbaad060e6bb90c0` | `neuronte-split-v5.nrt` |
| Terminal, 596 more Latin letters: Latin Extended-B, IPA, spacing modifiers, Latin Extended Additional, Greek Extended (2.3.0) | `neuronte-term-latin-v1.nrt` | 4,280,128 | `d54279bcbb190178f61a10c1d55f9840cadbd6f1b858a9993c232dc0106a473b` | none |
| Its list of characters (2.3.0) | `neuronte-term-latin-v1.list.txt` | 4,475 | `2edaf0d91269550dfcb4e951eebf6e9f9418ee7d343040af1c75bbc4f655541f` | none |
| Terminal, 465 arrows and mathematical operators (2.3.0) | `neuronte-term-math-v1.nrt` | 2,686,120 | `cf30e22efe80b233c73ca2f0c78490c153e172995187cdc1798206db647b1f77` | none |
| Its list of characters (2.3.0) | `neuronte-term-math-v1.list.txt` | 754 | `0dd409553fd188325a92ee0a33b2064da9f030eca23ed0768fb751fdea226a8a` | none |
| Terminal, 480 punctuation, letterlike, technical and geometric characters, and U+FFFD (2.3.0) | `neuronte-term-symbol-v1.nrt` | 2,743,740 | `5c6121177da923e9c8c05cc45960715b893db2b123712eb6c3e8891924ca2480` | none |
| Its list of characters (2.3.0) | `neuronte-term-symbol-v1.list.txt` | 1,015 | `fde0648dbe7c29f4b1a57efde4daba30c194b2c3899b87c64c4170a321133f95` | none |
| Terminal, 323 Miscellaneous Symbols and Dingbats (2.3.0) | `neuronte-term-dingbat-v1.nrt` | 2,000,188 | `0f2a6b1548c0b129ab127d6e23a96c3e7e631865f9bee37ead58513293eaf6b3` | none |
| Its list of characters (2.3.0) | `neuronte-term-dingbat-v1.list.txt` | 630 | `fe4cb65a64767ebb0b1340aa4425fa97626e4667e2f66fc2c85a04019aa92f71` | none |
| Terminal, 812 more letters of Latin, Greek and Cyrillic: the rest of Latin Extended-B, IPA and phonetic letters, small capitals, modifier letters, Latin Extended-C and -D, more Greek, Coptic and Cyrillic (2.4.0) | `neuronte-term-letters-v1.nrt` | 5,882,132 | `a5b23948026761ce58ed11b3542df38b842a6f1c93fd2c4971e60d9d4b7700fd` | none |
| Its list of characters (2.4.0) | `neuronte-term-letters-v1.list.txt` | 6,017 | `5ab408b55ab47d538fb39f6fe117b0123da360e89f7890b65b5e1fb452f415a8` | none |
| Terminal, 310 characters made by rule: 64 Yijing hexagrams, 87 Tai Xuan Jing figures, 100 domino tiles, 59 playing cards (2.4.0) | `neuronte-term-blocks-v1.nrt` | 849,696 | `8c3a73904ff0301eb96ea2a9d2ef6a926cc69406fe057298be093f248c2505c6` | none |
| Its list of characters (2.4.0) | `neuronte-term-blocks-v1.list.txt` | 424 | `4f3a727fd6fd8c1c809c0002b65916c8268b7f10c09663e1a2a8cb61bcef7f37` | none |
| Terminal, 179 letters and digits of the sans-serif, monospace and double-struck mathematical alphabets (2.4.0) | `neuronte-term-mathalpha-v1.nrt` | 971,056 | `b6bd7d73a1c2116b4383d70c810fe780ca3e36846c32c7178c1e5a2764167c57` | none |
| Its list of characters (2.4.0) | `neuronte-term-mathalpha-v1.list.txt` | 1,721 | `5b004506be1bb3724a5f8b531ec346bdd0693e006f3a825ae180f27ff361a8c1` | none |
| Terminal, 168 Canadian syllabics (2.4.0) | `neuronte-term-syllabics-v1.nrt` | 856,824 | `bb992b61ed2c31a6ae81240e5504a9adb95bf19d8a933f4066c149f37de5f5af` | none |
| Its list of characters (2.4.0) | `neuronte-term-syllabics-v1.list.txt` | 1,445 | `dbbe5d6efaf1082d96a7940bb7d6846e186cdf23e1e00609f5557ec9a66afaf8` | none |
| Terminal, the 48 characters of Lisu (2.4.0) | `neuronte-term-scripts-v1.nrt` | 343,188 | `d18eda009dfb6a955ca0a74e0f22f74f7f314d34a036ed331be44c581fb552e6` | none |
| Its list of characters (2.4.0) | `neuronte-term-scripts-v1.list.txt` | 593 | `9435f0e5e5d95d12eab0e8eeb779661edae60ebe63b87ef68db7f643d379d098` | none |
| Terminal, 197 characters: Hebrew letters, presentation forms and punctuation, Lao letters and vowel signs, the baht sign, faces and moon phases in one colour (2.4.0) | `neuronte-term-rest-v1.nrt` | 1,484,956 | `9a92c86c183fa9b435d89663780c643a2031f853e0096bd233e7bfdf431a4b6d` | none |
| Its list of characters (2.4.0) | `neuronte-term-rest-v1.list.txt` | 1,952 | `9c038bc0b95d6376ea75370560b875d5166ba2c161f418c546117ea2b077bda3` | none |
| The 39 symbols the Kilix terminal prints on its bars, one cell each (2.4.0) | `neuronte-kilix-chrome-v1.nrt` | 343,224 | `235e2e9d9ec83f83f72feb848df1fb1f47a7ca4d0b794cdf9d3763646cfb892c` | none |
| Its list of characters (2.4.0) | `neuronte-kilix-chrome-v1.list.txt` | 583 | `94088e432be843ad246a49791958675a56f189e474d796934ff50122a5f960c0` | none |
| 41 of those symbols drawn for a box of two cells (2.4.0) | `neuronte-kilix-chrome-v2.nrt` | 457,336 | `8583eadf252d499fe5b8b3ae5c28c4716cdd502c6f509f5a5c473dd033e9519c` | none |
| Its list of symbols (2.4.0) | `neuronte-kilix-chrome-v2.symbols.txt` | 901 | `9022dd947a1433a64fec6530df5f50316d9759dad3748332860d73d376ae032d` | none |
| Nine weather symbols in colour, two cells each, asked for by halves (2.4.0) | `neuronte-weather-colour-v1.nrt` | 396,956 | `517cad856fc4605fdfa12c7fa3847ca0b39e8a7dba2639ce3a6b0dbaac8965d2` | none |
| Its list of symbols (2.4.0) | `neuronte-weather-colour-v1.symbols.txt` | 509 | `e5cf7bda18e26bb06b7d2755c0bbfd71999e77a4b53871c3ebdb7fe2380f058f` | none |
| Emoji | `neuronte-emoji-v1.nrt` | 4,785,292 | `7aaccccf37175e3b5b973bb89b4731011308e6d1e6a8e8f12a1e5e6e5bc7973d` | `neuronte-emoji-v0.nrt` |
| Icons | `neuronte-icon-v2.nrt` | 496,184 | `0e85a2a45eaa99f555ca48e801a8a7c1094c9d2b791fec75594079c21f008f51` | `neuronte-icon-v1.nrt` |
| Kilix flame | `neuronte-flame-v1.nrt` | 57,372 | `eca551241b2f46934412c348a6e96bee13dafac214439a9c6d5c7bf0d11713bf` | `neuronte-flame-v0.nrt` |
| Pointer | `neuronte-pointer-v1.nrt` | 41,984 | `39a03fa8cc31ff9c0de0d0f9b8a02d8565a0c6f89ce499324418788c99116856` | `neuronte-pointer-v0.nrt` |
| Pointer manifest | `neuronte-pointer-v1.manifest.json` | 979 | `247341a3f7a89213b9cc26033e4ed392776393ca273f472ed713f8d867e92b35` | `neuronte-pointer-v0.manifest.json` |
| Video masks | `neuronte-video-masks-v0.nrt` | 358,996 | `d81fb4099a2f333e654b2f57a85ff0c13d57ca3dc7d5a4a942bc350650e4707c` | same file |
| Desktop face above 32 px, ASCII and all of Latin-1 (2.5.0) | `neuronte-ui-large-latin1-v2.nrt` | 2,946,196 | `f50ffdae9770686accf3e0fd79109487885977e06c4d1282102c5d9701dd3fc4` | `neuronte-ui-large-v0.nrt` |
| Desktop face, 17 to 32 px, upright and sheared italic, ASCII and all of Latin-1 (2.5.0) | `neuronte-ui-scaled-latin1-v1.nrt` | 3,250,312 | `e072cdf174cc8ee3d4bfe96f3840155e6ee74ba728c68f663464058367a21b3a` | `neuronte-ui17-v0.nrt` to `neuronte-ui32-v0.nrt` and their italics |
| Italic advances, 17 to 32 px (2.5.0) | `neuronte-ui-scaled-latin1-italic-advances-v1.nrt` | 1,568 | `d88bc12cffde171e7b9616666e707e43c2ecbef80b9fb7f1c8d823ad4cd57b46` | in each italic file |
| Desktop face, 1 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui1-v2.nrt` | 19,584 | `ddfc0319a28834a71468f66ed9cdb48bd4b64bbcbee1958c869745ab78c508be` | `neuronte-ui1-v0.nrt` |
| Desktop face, 2 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui2-v2.nrt` | 19,624 | `10176a6ecb902ac2913022a67d39ef90cad3d4eeaa6746535d98f4387a8fa1d7` | `neuronte-ui2-v0.nrt` |
| Desktop face, 3 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui3-v2.nrt` | 797,032 | `5a5aa0e5fa8b7215983d2670f79c7a3fa66eb206f50a1d9807d91b4d01473842` | `neuronte-ui3-v0.nrt` |
| Desktop face, 4 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui4-v2.nrt` | 1,372,508 | `635fddfaab366c1b09920115cb98f7c7a306dc4f5955e702ec79775e96feb80c` | `neuronte-ui4-v0.nrt` |
| Desktop face, 5 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui5-v2.nrt` | 797,032 | `4f4fa1b0525436b3967a9b5e33f17ed90168fa9ed0fc47afac0b0ac8b3ff09e6` | `neuronte-ui5-v0.nrt` |
| Desktop face, 6 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui6-v2.nrt` | 1,372,508 | `2c8e7b983dee30041116cdf52f5638aa011c84474afeaffa87d0e4f4f0711727` | `neuronte-ui6-v0.nrt` |
| Desktop face, 7 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui7-v2.nrt` | 797,032 | `5d93bfa4ee163f0a1e50f9d63820a94954b830391211d24b46ee14dcd8679dcf` | `neuronte-ui7-v0.nrt` |
| Desktop face, 8 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui8-v2.nrt` | 797,032 | `0f2833a3564593e728fe72ae2ce32d0f48fe9944e4b7eedefd160f797c727391` | `neuronte-ui8-v0.nrt` |
| Desktop face, 9 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui9-v2.nrt` | 1,372,508 | `f670a43fac6c6499801c1c6fb169fc06337046d5ed53416de0fe71633f898c92` | `neuronte-ui9-v0.nrt` |
| Desktop face, 10 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui10-v2.nrt` | 1,372,508 | `fcf127872b2fba9c9794a96c0ce957275a1b86c8f02298628fe38a28afa2635b` | `neuronte-ui10-v0.nrt` |
| Desktop face, 11 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui11-v2.nrt` | 1,372,508 | `f6453c83433fb3d24ec92c54a82a04ef89b95fe164f1b0fbecfda47d8efa1c07` | `neuronte-ui11-v0.nrt` |
| Desktop face, 12 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui12-v2.nrt` | 1,627,484 | `81dc0abf09e73b64166b59c00be20924f4da7fad6745e8104e0cba558382ff2f` | `neuronte-ui12-v0.nrt` |
| Desktop face, 13 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui13-v2.nrt` | 1,372,508 | `e8b1f6d5b257e326992570403718ecdaf8ea2126dc89712afb7ed372614fda9b` | `neuronte-ui13-v0.nrt` |
| Desktop face, 14 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui14-v2.nrt` | 1,372,508 | `3a3c3f4b962ab5555872490c0b7e4fb5dbc0b6dd0c63cff2680ff491b3fa71d4` | `neuronte-ui14-v0.nrt` |
| Desktop face, 15 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui15-v2.nrt` | 1,648,636 | `43c67e9102a8b73f9d3d6283a31f1a0f086bb0beca0184de14cdfab7e15776d7` | `neuronte-ui15-v0.nrt` |
| Desktop face, 16 px, ASCII and Latin-1 (2.5.0) | `neuronte-ui16-v2.nrt` | 709,864 | `0e3555fa4929635a5cadfb96a8e3ae719eaa07588b6cc86095d3f10d28d8d503` | `neuronte-ui16-v0.nrt` |
| Desktop face, 3 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui3-italic-v2.nrt` | 797,032 | `355348fd83ad313eeaf7016d4da8eb8f6d930b8907ef36af436b53f1519d014d` | `neuronte-ui3-italic-v0.nrt` |
| Desktop face, 4 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui4-italic-v2.nrt` | 797,032 | `0e755326af0dd49dbc2a711141a48cc7cade67259bd42c88645b5c19855a37f1` | `neuronte-ui4-italic-v0.nrt` |
| Desktop face, 5 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui5-italic-v2.nrt` | 1,372,508 | `6e6cf387987c377b257b9b48f91fc25eb2ca54bff92fb6894155af3ce7873bfe` | `neuronte-ui5-italic-v0.nrt` |
| Desktop face, 6 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui6-italic-v2.nrt` | 1,372,508 | `60337ae248c6b2fe0f67889db944b4fced8d081c89e55b637496bf01ef65ff6b` | `neuronte-ui6-italic-v0.nrt` |
| Desktop face, 7 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui7-italic-v2.nrt` | 1,372,508 | `75eeeabb184c9442ddd807adec1cb969acbda55d1f4ba5df1e5fab24f2be80bd` | `neuronte-ui7-italic-v0.nrt` |
| Desktop face, 8 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui8-italic-v2.nrt` | 797,032 | `17b99d7e53423f5fe539c50bb2edd5665e30d134fafe0a612c16bb0b06178300` | `neuronte-ui8-italic-v0.nrt` |
| Desktop face, 9 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui9-italic-v2.nrt` | 1,372,508 | `fcc7cb131fde04baec050ca1a151b30bdaf9fc2e1df81b2987e902b715dc2d2a` | `neuronte-ui9-italic-v0.nrt` |
| Desktop face, 10 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui10-italic-v2.nrt` | 1,393,660 | `51003834f48151b013afceaafe1155edd7f2c5e6e1c082292f052bba7cc8c03a` | `neuronte-ui10-italic-v0.nrt` |
| Desktop face, 11 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui11-italic-v2.nrt` | 1,393,660 | `c5e1956cbe4fbdd5fe52d6d836810db0af16eeeaffd937846c72872b8c2635fa` | `neuronte-ui11-italic-v0.nrt` |
| Desktop face, 12 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui12-italic-v2.nrt` | 1,627,484 | `acb59a964d7928ee251cb795ccaec1d47a3fd7775f65641c118dd34b3f1ab11a` | `neuronte-ui12-italic-v0.nrt` |
| Desktop face, 13 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui13-italic-v2.nrt` | 640,488 | `64161fa95eec6c81c1d3f349abf02cf2c948952017238d226a04ce795bbf1c56` | `neuronte-ui13-italic-v0.nrt` |
| Desktop face, 14 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui14-italic-v2.nrt` | 1,372,508 | `1d55d5a0cd596de4b9344dc24e7631bf2993109cd9f59c8e0b11a6ff46383296` | `neuronte-ui14-italic-v0.nrt` |
| Desktop face, 15 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui15-italic-v2.nrt` | 1,414,812 | `8f5e8f4a9814ac2975ee18488108d68bd84f92e4154f3b91163ba636ce064254` | `neuronte-ui15-italic-v0.nrt` |
| Desktop face, 16 px italic, ASCII and Latin-1 (2.5.0) | `neuronte-ui16-italic-v2.nrt` | 619,336 | `ba6f648535196ef8337f150ea48e2ea7ac5341343b8047df01e85cb3aa87c433` | `neuronte-ui16-italic-v0.nrt` |
| The 103 fullwidth forms, drawn for a box of two cells (2.6.0) | `neuronte-wide-forms-v1.nrt` | 685,308 | `9c1f6ea1791d2a9119fd124fc1c6073f953fe9b4ba3f340acb3605bb3119db1b` | none |
| Its list of characters (2.6.0) | `neuronte-wide-forms-v1.symbols.txt` | 1,321 | `e3c7073553827b33c35d509967ae16357a4972f2582593e882106211569009ce` | none |
| 282 wide signs below U+1F300 for a box of two cells; 29 of its nets FAILED their limits (2.6.0) | `neuronte-wide-symbols-v1.nrt` | 4,166,204 | `66074bfc548dd6f572c121d0848f38421d626b125da4c0e948346e6144fed3d1` | none |
| Its list of characters (2.6.0) | `neuronte-wide-pictographs2-v1.symbols.txt` | 3,427 | `ac0e63e659735e40749624673d4146df3e2ec902d4de1f63450f926fdef8e61a` | none |
| Its list of characters (2.6.0) | `neuronte-wide-pictographs-v1.symbols.txt` | 3,626 | `028c1c287de4f9c898d7b710a491f2772dccd1dff455ae65e151090b64a33242` | none |
| Its list of characters (2.6.0) | `neuronte-wide-supplement-v1.symbols.txt` | 4,296 | `2be93b7ff5d41580f728735105e6ff63e75666e2f9e5cc57f92fa587bc7e3090` | none |
| Its list of characters (2.6.0) | `neuronte-wide-symbols-v1.symbols.txt` | 3,565 | `387c2fd257752c624d866f404f4d7f6b3605cc35415a571d6ceff3bdde316af8` | none |
| 273 pictographs, U+1F300..1F448, for a box of two cells; 27 of its nets FAILED (2.6.0) | `neuronte-wide-pictographs-v1.nrt` | 4,451,088 | `fd6a943eed3366653911a4ec9be9915f09edd540df3316cf1948270bbc2917d2` | none |
| 266 pictographs, U+1F449..1F5FF, for a box of two cells; 16 of its nets FAILED (2.6.0) | `neuronte-wide-pictographs2-v1.nrt` | 3,880,504 | `692fe8a5a4928ebcc1130f376dcfef401e72fba81d50e5fa1abbfcfd6a87999e` | none |
| 337 supplemental and extended-A pictographs for a box of two cells; 23 of its nets FAILED (2.6.0) | `neuronte-wide-supplement-v1.nrt` | 5,192,780 | `08b85a02c63026c47ae0ddcbb38226db4054111ded94d1eeda4d8c9517b8cec9` | none |
| Neuronte Sans Metric, 1 px (2.6.0) | `neuronte-metric-sans1-v1.nrt` | 19,596 | `122d7f65c04a7c04c6218648a88ce27532a1aade0ec5bd43c5431b6979853e9e` | none |
| Neuronte Sans Metric, 2 px (2.6.0) | `neuronte-metric-sans2-v1.nrt` | 19,636 | `4c4708ea4420b2815c19593654851855933bffc2eba9272fbb6a591a11b82894` | none |
| Neuronte Sans Metric, 3 px (2.6.0) | `neuronte-metric-sans3-v1.nrt` | 1,016,948 | `11dd957f42b398e8ce78aa6e2e77618f4b357b6910d56637ae688f516555d59b` | none |
| Neuronte Sans Metric, 3 px italic (2.6.0) | `neuronte-metric-sans3-italic-v1.nrt` | 1,016,948 | `0c6cefa1fb0645c58b8c57e8359b318656e16396757d486ab7ba3b3fe05d23c1` | none |
| Neuronte Sans Metric, 4 px (2.6.0) | `neuronte-metric-sans4-v1.nrt` | 1,016,948 | `80f85041d8b4a7c02470b0aefe829240f5bd1ca853a6aced2028dbf079a4fe47` | none |
| Neuronte Sans Metric, 4 px italic (2.6.0) | `neuronte-metric-sans4-italic-v1.nrt` | 1,016,948 | `df57b0d187406802996f8f0de630534e2f556cdc402644d80b7c71a8baf6c966` | none |
| Neuronte Sans Metric, 5 px (2.6.0) | `neuronte-metric-sans5-v1.nrt` | 1,016,948 | `5748d7bdcf3130d0815950c6bddc31143b97afe23665f9e3d6c2bc14fc4bae8b` | none |
| Neuronte Sans Metric, 5 px italic (2.6.0) | `neuronte-metric-sans5-italic-v1.nrt` | 1,016,948 | `e0c6e57a02d5396300d9cbcdc2e070b76987cd48cdba4c566a18078173fe0ed2` | none |
| Neuronte Sans Metric, 6 px (2.6.0) | `neuronte-metric-sans6-v1.nrt` | 1,016,948 | `2f4bc3e229566f9d2de60002693b6a4a0317f3e8e2a2e2fbe953cae87781b32b` | none |
| Neuronte Sans Metric, 6 px italic (2.6.0) | `neuronte-metric-sans6-italic-v1.nrt` | 1,016,948 | `3c6b82b1cbf96043836af849f6866f33dc16f6d208d6c310b3585ee52d081e1c` | none |
| Neuronte Sans Metric, 7 px (2.6.0) | `neuronte-metric-sans7-v1.nrt` | 1,016,948 | `e64c40d51e9c487805d9e3b8307e2f5c5075950ba3e00b43354ed1313c6bbfaf` | none |
| Neuronte Sans Metric, 7 px italic (2.6.0) | `neuronte-metric-sans7-italic-v1.nrt` | 1,016,948 | `47bd60eec234a8bd03c18b7ac8aba762b3bd0b6f09560b1a3ff193de75f9d9ca` | none |
| Neuronte Sans Metric, 8 px (2.6.0) | `neuronte-metric-sans8-v1.nrt` | 1,016,948 | `8a8851b2d6e7c4cfcafdd93d8be7080bc0d9a53f1c40ba876f9e7f2e624cc1ea` | none |
| Neuronte Sans Metric, 8 px italic (2.6.0) | `neuronte-metric-sans8-italic-v1.nrt` | 1,016,948 | `1703fcd3dc401e8d96d105cb90dc1c72dfcc2a17b273eaedb8a94970fceaf86c` | none |
| Neuronte Sans Metric, 9 px (2.6.0) | `neuronte-metric-sans9-v1.nrt` | 1,016,948 | `0de9ed9d22a370c9ef9029d8faae363d25423404b59887e595d32a1e33647c4d` | none |
| Neuronte Sans Metric, 9 px italic (2.6.0) | `neuronte-metric-sans9-italic-v1.nrt` | 1,016,948 | `ef0e59da443b29c63890d260699234cb41eefcb28a8a440ebcc64f3604e93103` | none |
| Neuronte Sans Metric, 9 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans9-bold-v1.nrt` | 1,016,948 | `22cea03302fda3e4e29a1e0c371d2de35ae5e16eccc3ef7a6dde7c54a0de52db` | none |
| Neuronte Sans Metric, 9 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans9-bold-italic-v1.nrt` | 1,016,948 | `a5bade46c544840ef9072cd83e85029cab6e2444edff81b5b90df2027924782d` | none |
| Neuronte Sans Metric, 10 px (2.6.0) | `neuronte-metric-sans10-v1.nrt` | 1,016,948 | `84bb9db12080353182c990055effb1fddb542a12fab5a3d73da95a4c5d1a558e` | none |
| Neuronte Sans Metric, 10 px italic (2.6.0) | `neuronte-metric-sans10-italic-v1.nrt` | 1,016,948 | `226937af26f6ff2ed56893548484230d2bb507d16d6cc2e49876e9934fdf9005` | none |
| Neuronte Sans Metric, 10 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans10-bold-v1.nrt` | 1,016,948 | `bdd2e884cc5619acc08a29704de22f064b143eacfa5e3c41d96b900df269fcc3` | none |
| Neuronte Sans Metric, 10 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans10-bold-italic-v1.nrt` | 1,016,948 | `a2f16b8d88331d9a6229227ed748f93478f76751593e796b9cdefe17aa894d17` | none |
| Neuronte Sans Metric, 11 px (2.6.0) | `neuronte-metric-sans11-v1.nrt` | 1,016,948 | `e0431d873b2e5c8e4266d7ed76e21a403cec08ab829de5e631b92b10c31562f8` | none |
| Neuronte Sans Metric, 11 px italic (2.6.0) | `neuronte-metric-sans11-italic-v1.nrt` | 1,016,948 | `495f459cf8a95f51fc72b339a9388527d07f40ce26c264e3896b4bb1a52e8ae8` | none |
| Neuronte Sans Metric, 11 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans11-bold-v1.nrt` | 1,016,948 | `441260fe9ff64cb7e63bb2ce1860ae62980991efa7163e8a2a4295d5035ce6dc` | none |
| Neuronte Sans Metric, 11 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans11-bold-italic-v1.nrt` | 1,016,948 | `4bc5303c2874ac7da7da08426b7376ce87ea4b6199a128d92a91933e47c1e3c9` | none |
| Neuronte Sans Metric, 12 px (2.6.0) | `neuronte-metric-sans12-v1.nrt` | 1,016,948 | `2ec1cd7642482310533b456c74ac827b0772a9bccf5dbdefe2b0d97a9199bd06` | none |
| Neuronte Sans Metric, 12 px italic (2.6.0) | `neuronte-metric-sans12-italic-v1.nrt` | 1,016,948 | `fb1ae20bf93ebc0be8280958fde45c703af792f362cfecbbcb21c3a785357e5a` | none |
| Neuronte Sans Metric, 12 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans12-bold-v1.nrt` | 1,016,948 | `e4eade8c9e1376cb782981fc38297c5c0b227791e81028b88882c7df02ed7faf` | none |
| Neuronte Sans Metric, 12 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans12-bold-italic-v1.nrt` | 1,016,948 | `4b8c031faf38717c4aaa92db5399cec632271ea80ebfe1fa774b24840c7e7cc1` | none |
| Neuronte Sans Metric, 13 px (2.6.0) | `neuronte-metric-sans13-v1.nrt` | 1,016,948 | `b9d1c9faf7f0a6e587bd719aca94bda3e2e2f509eb22d83e7e988306857bb86b` | none |
| Neuronte Sans Metric, 13 px italic (2.6.0) | `neuronte-metric-sans13-italic-v1.nrt` | 1,016,948 | `aa71669a03456e507981c4bf86bc225bccdf28b529917d26e176fc87fd2bf65d` | none |
| Neuronte Sans Metric, 13 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans13-bold-v1.nrt` | 1,016,948 | `cec5c68db30078ed19ab75a77b8b068895176da83ba2f692cf5c5a3f94afa91d` | none |
| Neuronte Sans Metric, 13 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans13-bold-italic-v1.nrt` | 1,016,948 | `44e6a58079325b96ee6070b441a2075ec32931e374f0bbb46fe4f10399924ee9` | none |
| Neuronte Sans Metric, 14 px (2.6.0) | `neuronte-metric-sans14-v1.nrt` | 1,016,948 | `e608d6cd723ad5470e843ed55e22508560c3fb7ec1c2531c2db50c26f41eeb50` | none |
| Neuronte Sans Metric, 14 px italic (2.6.0) | `neuronte-metric-sans14-italic-v1.nrt` | 1,016,948 | `d4eaf48805da6703caa297f3bcc48f0cde3d22e312e5ddacba5fc4cff5018405` | none |
| Neuronte Sans Metric, 14 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans14-bold-v1.nrt` | 1,016,948 | `483d75cfc6e20cb142b34983c48cd798c9448c0fe14b96176f4b61d885524801` | none |
| Neuronte Sans Metric, 14 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans14-bold-italic-v1.nrt` | 1,016,948 | `81e6c0f0f2e8009484547c1c184636e5860ffb622c8d44290972159b3e60d1fc` | none |
| Neuronte Sans Metric, 15 px (2.6.0) | `neuronte-metric-sans15-v1.nrt` | 1,016,948 | `f4a132c24b55a51ab0ad2e03abb2bec14e198a7e361a484cedc6b1ace9dc170b` | none |
| Neuronte Sans Metric, 15 px italic (2.6.0) | `neuronte-metric-sans15-italic-v1.nrt` | 1,016,948 | `ce52bdbcd9621f6b36c6095f4422c2eade1fb503d6e656c4e9a45da55d502465` | none |
| Neuronte Sans Metric, 15 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans15-bold-v1.nrt` | 1,038,100 | `a6399e16eb88a704ae87a73029ebefb15a6e97da27eef84ad69f08abbf013f02` | none |
| Neuronte Sans Metric, 15 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans15-bold-italic-v1.nrt` | 1,038,100 | `96e3f3f4a5dd285daf3706da309fdad6d2fb76bb6fcf14d4ccddb56b4cdc266c` | none |
| Neuronte Sans Metric, 16 px (2.6.0) | `neuronte-metric-sans16-v1.nrt` | 1,016,948 | `acd7cf29827d2c69f30afd3416f4ce2b471cdbb2b744167490152d740aea9642` | none |
| Neuronte Sans Metric, 16 px italic (2.6.0) | `neuronte-metric-sans16-italic-v1.nrt` | 1,016,948 | `46e24c786cf1c522ae8d423589dbbb82fc14cd560576f778c7f1c75fded81561` | none |
| Neuronte Sans Metric, 16 px bold, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans16-bold-v1.nrt` | 1,016,948 | `badb6d40188029d067b47ff8923d231b00abc5fa1e12cd4b29f5dbd821af5a22` | none |
| Neuronte Sans Metric, 16 px bold italic, drawn bold: draw with bold 0 (2.6.0) | `neuronte-metric-sans16-bold-italic-v1.nrt` | 1,038,100 | `1317333cb86de4de7d0eac5c3759748531a1254d97c1ad859d0445c9788f073c` | none |
| Neuronte Sans Metric, 17 to 32 px, upright and sheared italic; FAILED its declared gate G-M1, shipped by the owner's ruling (2.6.0) | `neuronte-metric-sans-scaled-v1.nrt` | 4,423,128 | `7e059b101710afcfb6f50bb559b99070f20cafe1ae5985f2fafb8e42f75f2e0c` | none |
| Neuronte Sans Metric above 32 px; FAILED its declared gate G-M1, shipped by the owner's ruling (2.6.0) | `neuronte-metric-sans-large-v1.nrt` | 4,418,804 | `5a2fd71c121c4b1f2b463ae306c18ecdf3cc1ef66159049f90254c279641668d` | none |
