## Installation

1. Télécharger le projet
2. Extraire le dossier release
4. Entrer dans le dossier et lancer le fichier exécutable `VulkanTuto - triangle.exe`

## Les fonctionnalités
#### Initialisation et affichage
- Une fenêtre GLFW se crée
- Le programme vérifie l'API
- Le shader slang est compilé en SPIR-V avec compile.bat


#### Géométrie et transformations
- Une figure géométrique peut s'afficher grâce aux info des positions, couleurs et UV dans le fichier Mesh.h
- Une caméra positionner de façon fixe


#### Textures
- Une texture se charge depuis un fichier avec stb_image
- L'image s'affiche sur le cube


#### Profondeur
- Depth buffer est intégré 

#### Gestion de la fenêtre et arrêt
- Swapchain, image views et depth buffer sont recréés.

### ceux qui manquent
#### Initialisation et affichage
- Pas de message d'erreur exploitable

#### Géométrie et transformations
- La rotation de la figure géométrique ne peut pas être modifier sans toucher au renderer

#### Gestion de la fenêtre et arrêt
- La fenêtre ne peut pas être redimensionner
