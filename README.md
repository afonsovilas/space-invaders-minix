# Space Invaders on MINIX
 
A version of the classic arcade game *Space Invaders*, written in C and running directly on the MINIX 3 operating system. Instead of using high-level libraries, the game talks to the hardware through device drivers developed by the team.
 
Developed for the **Computer Laboratory (Laboratório de Computadores, LCOM)** course of the Bachelor in Informatics and Computing Engineering at the Faculty of Engineering of the University of Porto (FEUP), 2025/2026.
 
## Getting started
 
### Requirements
- MINIX 3 virtual machine set up for LCOM
- LCOM framework (`liblcf`)
### Build and run
Inside the MINIX virtual machine, from the project's source folder:
 
```bash
make
lcom_run proj
```
 
### How to play
- [Key/mouse]: move the ship
- [Key/mouse]: shoot
- [Key]: pause / exit
## Features
 
- Player ship, waves of enemies, shooting and collisions
- Score and game over screen
- Graphics drawn directly to the video memory
## Devices used
 
| Device | Use in the game |
|---|---|
| Timer | Game loop and frame rate |
| Keyboard | Player controls and menus |
| Mouse | [Use in the game] |
| Video card | Graphics mode and drawing of sprites |
 
## License
 
This project was developed for academic purposes and is shared as part of the authors' portfolio. All rights reserved by the authors.
 
## Declaration of Responsible AI Use
 
We declare that:
 
1. We are responsible for all code and documentation in this repository, and we understand that we must be able to explain and justify any part of it on request.
2. We have used AI-based tools (e.g., code assistants, chatbots, or generators) only to support our learning, not to bypass the intended learning outcomes or any assessment rules.
3. Wherever AI tools contributed to this work, we have:
   - Used them within the limits set by the course policies and institutional regulations.
   - Reviewed, tested, and, where necessary, edited the outputs, taking full responsibility for their correctness, originality, and legality.
   - Ensured that no confidential, personal, or sensitive data were shared with AI tools.
4. We have not used AI tools to generate complete solutions that we present as entirely our own unaided work, and we have avoided plagiarism, whether from AI outputs or other sources.
5. If asked, we will provide details of which tools we used, for which files or parts of the project, and how we verified and adapted their outputs.
Signed: Rui Moreira, Rodrigo Almeida, Luís Castanheira, Afonso Vilas
Date: 5 June 2026
 
## Authors and acknowledgment
 
LCOM project for group GRUPO_2LEIC02_1.
 
Group members:
- Rui Moreira
- Rodrigo Almeida
- Luís Castanheira
- Afonso Vilas
 
