# The shipped files

66 files, 72,026,486 bytes. Every file is a file of Neuronte, copied byte for byte under the same name: the 33 desktop faces marked 2.5.0 are files of Neuronte at commit [bfd0a2d](https://github.com/itsmygithubacct/neuronte/tree/bfd0a2d066161eaa5abeae9747e151ff345702ef) (branch `work/neuronte-latin1-faces`; no Neuronte tag holds them yet), and every other file is a file of [Neuronte 0.1.6](https://github.com/itsmygithubacct/neuronte/tree/v0.1.6). Each 2.5.0 face is the file it replaces (the ASCII-only `-v1` face of the same size, `neuronte-ui-scaled-latin1-v0` with its italic advances `-v0`, and `neuronte-ui-large-latin1-v1`) with nets for the rest of Latin-1 added beside its unchanged nets. Of the others, all but the eighteen marked 2.4.0 are also files of 0.1.5, and all but those and the eight marked 2.3.0 are files of 0.1.4. The four bundles marked 2.3.0 are opened beside the terminal bundle, each with the list of the characters it was trained on; so are the six terminal bundles marked 2.4.0. The three bar bundles marked 2.4.0 are for a terminal's own bars. The three desktop files for 17 px and up do the work of 33 files of 0.1.2, as the three of 2.1.1 they replace did, and the terminal bundle marked 2.2.0 takes the place of `neuronte-split-v8.nrt`. The last column is the file of 0.1.1 that did the same job.

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
