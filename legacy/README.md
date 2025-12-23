# Legacy - Decentralized File Storage Network
This folder contains, parts of the old codebase (prior to the commit "[The Great Purge](https://github.com/B4S1C-Coder/Decentralized-File-Storage/commit/a21aab40843e35ac1d4ac2f7630d9cc99cc3d637)". The current codebase has evolved directly from the old codebase.

>**TLDR**: This folder contains old code, just for reference and is no longer part of the actual project. But still part of the codebase nonetheless.

## Why the great purge?
I had initially started the project in the first year of my undergrad in 2023. By end of 2023 and early 2024 I was frustrated by the state of the project and straight up nuked the repo out of existence. After a few months I realized I should not have done that and found out you *could* restore deleted repos, so I did. But the dumb me, decided to use the existing faulty local repo, I tried to push and naturally git didn't let me. At the time I didn't know how to use git properly so I force pushed and that nuked the commit history prior to the great purge.

## Inital Design
Initially, the project was supposed to be python based with it's core written in C. I even wrote interface layers, to easily import this C code (and later C++ versions) into Python via CDLL module. I realized I wanted to be cool and use the C++ features like smart pointers etc. So I wrote the entire thing in C++ and addressed several *safety* issues in the C versions.

>**Note**: Current C++ Safety is based on the analysis done by Infer by Meta for static code analysis.

Now the project is purely C++ 23.
