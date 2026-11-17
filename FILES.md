# The shipped files

48 files, 44,549,249 bytes. Every file is a file of [Neuronte 0.1.5](https://github.com/itsmygithubacct/neuronte/tree/v0.1.5), copied byte for byte under the same name; all but the eight marked 2.3.0 are also files of 0.1.4. The four bundles marked 2.3.0 are opened beside the terminal bundle, each with the list of the characters it was trained on. All but four were already in Neuronte 0.1.2; the three marked 2.1.1 do the work of 33 files of 0.1.2, and the terminal bundle marked 2.2.0 takes the place of `neuronte-split-v8.nrt`. The last column is the file of 0.1.1 that did the same job.

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
| Emoji | `neuronte-emoji-v1.nrt` | 4,785,292 | `7aaccccf37175e3b5b973bb89b4731011308e6d1e6a8e8f12a1e5e6e5bc7973d` | `neuronte-emoji-v0.nrt` |
| Icons | `neuronte-icon-v2.nrt` | 496,184 | `0e85a2a45eaa99f555ca48e801a8a7c1094c9d2b791fec75594079c21f008f51` | `neuronte-icon-v1.nrt` |
| Kilix flame | `neuronte-flame-v1.nrt` | 57,372 | `eca551241b2f46934412c348a6e96bee13dafac214439a9c6d5c7bf0d11713bf` | `neuronte-flame-v0.nrt` |
| Pointer | `neuronte-pointer-v1.nrt` | 41,984 | `39a03fa8cc31ff9c0de0d0f9b8a02d8565a0c6f89ce499324418788c99116856` | `neuronte-pointer-v0.nrt` |
| Pointer manifest | `neuronte-pointer-v1.manifest.json` | 979 | `247341a3f7a89213b9cc26033e4ed392776393ca273f472ed713f8d867e92b35` | `neuronte-pointer-v0.manifest.json` |
| Video masks | `neuronte-video-masks-v0.nrt` | 358,996 | `d81fb4099a2f333e654b2f57a85ff0c13d57ca3dc7d5a4a942bc350650e4707c` | same file |
| Desktop face above 32 px (2.1.1) | `neuronte-ui-large-latin1-v1.nrt` | 2,209,684 | `8fec97a302697a703e15b9c59d90a2ce542a9dd98d8e9fe11a533eabe2623747` | `neuronte-ui-large-v0.nrt` |
| Desktop face, 17 to 32 px, upright and sheared italic (2.1.1) | `neuronte-ui-scaled-latin1-v0.nrt` | 2,512,896 | `13996f26dba65074fffa089b85440738ece4a8ef56ba6ac216c93ada38abbd2b` | `neuronte-ui17-v0.nrt` to `neuronte-ui32-v0.nrt` and their italics |
| Italic advances, 17 to 32 px (2.1.1) | `neuronte-ui-scaled-latin1-italic-advances-v0.nrt` | 1,264 | `8da323678a6d711ec6b30343798b7959dfcc3b0ba603c771aa63f172dd3ff605` | in each italic file |
| Desktop face, 1 px | `neuronte-ui1-v1.nrt` | 9,788 | `46117cf3fa86973d2819f90e75d9277625eaf787b5059a021eeba3e94a766a9b` | `neuronte-ui1-v0.nrt` |
| Desktop face, 2 px | `neuronte-ui2-v1.nrt` | 9,808 | `79ca6e44c50a9d16d7bfeb1803609698be25039bf1556cd3c2f0a8d2726f541b` | `neuronte-ui2-v0.nrt` |
| Desktop face, 3 px | `neuronte-ui3-v1.nrt` | 288,616 | `c5dde5a7fad867441a170e0f07201294e8aa9c64b52b53459ace58a648d11b38` | `neuronte-ui3-v0.nrt` |
| Desktop face, 4 px | `neuronte-ui4-v1.nrt` | 864,092 | `104009b40e721f1c0a6c5e56cb9ce4fd3ab2be02c6dc9b9f42addfc4f18a8a7f` | `neuronte-ui4-v0.nrt` |
| Desktop face, 5 px | `neuronte-ui5-v1.nrt` | 288,616 | `0c85e8b3351ac70c90e62bea4052834d6dd1bed0ee3b856e5f9cf323d83169cd` | `neuronte-ui5-v0.nrt` |
| Desktop face, 6 px | `neuronte-ui6-v1.nrt` | 864,092 | `30daf32feffe428a2ac902423b7702d7439b4e2146787a2e39812fe1ee76993b` | `neuronte-ui6-v0.nrt` |
| Desktop face, 7 px | `neuronte-ui7-v1.nrt` | 288,616 | `7f85c8c18da9fd779bcca10b357cede44f383a66018fa9055a932f711e459a0e` | `neuronte-ui7-v0.nrt` |
| Desktop face, 8 px | `neuronte-ui8-v1.nrt` | 288,616 | `ad95e3213316ff3e52adf5f3f69a4ce06d29ac5d78227c8f8411162e9aba1712` | `neuronte-ui8-v0.nrt` |
| Desktop face, 9 px | `neuronte-ui9-v1.nrt` | 864,092 | `d4823ab357b474334ca8c4ce958bc221e8e6b894fcddb8e708a6ce757fbdca6c` | `neuronte-ui9-v0.nrt` |
| Desktop face, 10 px | `neuronte-ui10-v1.nrt` | 864,092 | `03750ca4329b7d0f1d18647f1e910a910c90f996b76bb57c7b78890f2f3a8e6f` | `neuronte-ui10-v0.nrt` |
| Desktop face, 11 px | `neuronte-ui11-v1.nrt` | 864,092 | `8b0d533be90d6405a3fc1f040891a5fb3862113a7832c5d00aded7fafd068b4c` | `neuronte-ui11-v0.nrt` |
| Desktop face, 12 px | `neuronte-ui12-v1.nrt` | 1,119,068 | `cc97917f5aefa68f4be9f9e6fa5882221e22bd8f5d12eac747e901ca7dde5d9f` | `neuronte-ui12-v0.nrt` |
| Desktop face, 13 px | `neuronte-ui13-v1.nrt` | 864,092 | `c624785f91f611df2d9ecfe9b5c325aa47be0d7ad3e0d11535dc923ac2ab557e` | `neuronte-ui13-v0.nrt` |
| Desktop face, 14 px | `neuronte-ui14-v1.nrt` | 864,092 | `5ee5360ad9a60d75e7fce90fe86a5de265fc31b1f50d720ceb4b0efe71ac86d8` | `neuronte-ui14-v0.nrt` |
| Desktop face, 15 px | `neuronte-ui15-v1.nrt` | 1,119,068 | `da0bdcc73e76438947f573ea8e7f87ab0a64db2e7330de2853fac7743d9c3ccb` | `neuronte-ui15-v0.nrt` |
| Desktop face, 16 px | `neuronte-ui16-v1.nrt` | 180,240 | `e6dda51145ac963cbe07c327acd42d112001f73913199686b374a7dea88f9e0b` | `neuronte-ui16-v0.nrt` |
| Desktop face, 3 px italic | `neuronte-ui3-italic-v1.nrt` | 288,616 | `d5e921cf00c3a3746899e16214cc63a3aa018543ac9a80d77f02953e44885a01` | `neuronte-ui3-italic-v0.nrt` |
| Desktop face, 4 px italic | `neuronte-ui4-italic-v1.nrt` | 288,616 | `a73e3d8422b8772ee74b40195f5ff3ac32cb2eb200e4c293d3a4c00dbb1d8e54` | `neuronte-ui4-italic-v0.nrt` |
| Desktop face, 5 px italic | `neuronte-ui5-italic-v1.nrt` | 864,092 | `0e535af95c074a80efeba7879c436d960facfec209f7ec7f4dad9f735125a167` | `neuronte-ui5-italic-v0.nrt` |
| Desktop face, 6 px italic | `neuronte-ui6-italic-v1.nrt` | 864,092 | `289f7cfc6ca081324b8e1431119a874265ca390b45617e90ccc7db11979e6de6` | `neuronte-ui6-italic-v0.nrt` |
| Desktop face, 7 px italic | `neuronte-ui7-italic-v1.nrt` | 864,092 | `c1070e1bab6c495be6ede69a2b9689edd5084a84b8b1c324fcbd6815a5aef8f5` | `neuronte-ui7-italic-v0.nrt` |
| Desktop face, 8 px italic | `neuronte-ui8-italic-v1.nrt` | 288,616 | `931cbbf284e797942795d588aa65c176da3ca1c89ae1d81b56947879a5cf97fb` | `neuronte-ui8-italic-v0.nrt` |
| Desktop face, 9 px italic | `neuronte-ui9-italic-v1.nrt` | 864,092 | `496ecf1ca74f87a2ae6536d74bc6923fcf6bb0a8f3ce03a684705dcd0a826572` | `neuronte-ui9-italic-v0.nrt` |
| Desktop face, 10 px italic | `neuronte-ui10-italic-v1.nrt` | 864,092 | `f3e609cc984688ef91c956225282e1966e84113b89b0bcff66f230aff78a462b` | `neuronte-ui10-italic-v0.nrt` |
| Desktop face, 11 px italic | `neuronte-ui11-italic-v1.nrt` | 864,092 | `794fe10780301c0ce4e6c639378c2a09dc3c4b219ebd49366922342b09d7a1c8` | `neuronte-ui11-italic-v0.nrt` |
| Desktop face, 12 px italic | `neuronte-ui12-italic-v1.nrt` | 1,119,068 | `73b54e08ff5e5e5dc2b36b41c0323c3cb8034711ab70383382a2ef1ed2448889` | `neuronte-ui12-italic-v0.nrt` |
| Desktop face, 13 px italic | `neuronte-ui13-italic-v1.nrt` | 110,864 | `08d75e0c1098029d17ac24bcc1f9247ad4818d54c38ef41291426e7acd10137d` | `neuronte-ui13-italic-v0.nrt` |
| Desktop face, 14 px italic | `neuronte-ui14-italic-v1.nrt` | 864,092 | `388888776e74263ab9866bc60dae7d0ad901468f7106a85b20926051a9304861` | `neuronte-ui14-italic-v0.nrt` |
| Desktop face, 15 px italic | `neuronte-ui15-italic-v1.nrt` | 864,092 | `b195b61accb7caae7fb1725d26569c3b813bd0c02c5644733db1df741953553d` | `neuronte-ui15-italic-v0.nrt` |
| Desktop face, 16 px italic | `neuronte-ui16-italic-v1.nrt` | 110,864 | `198b6eaa64037d6abd9fd40268cfc4553056385b519b14546a91145a1408fd1f` | `neuronte-ui16-italic-v0.nrt` |
