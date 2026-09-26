# FuxEngine

FuxEngine est un moteur 3D expérimental écrit en C++20 avec OpenGL. Le dépôt sert à construire progressivement les systèmes d’un moteur et à les observer dans une scène de démonstration.

## État du projet

Le moteur sait actuellement créer une fenêtre, charger une scène, afficher des meshes texturés et éclairés, déplacer une caméra et simuler quelques collisions rigides. Le point d’entrée `src/FuxEngine.cpp` configure le playground physique utilisé pour observer ces fonctions.

Le projet reste un prototype d’apprentissage : certaines parties sont volontairement simples et ne sont pas encore adaptées à une grande scène ou à un jeu complet. Les limites connues sont listées plus bas.

## Architecture actuelle

Les en-têtes publics sont dans `include/` et les implémentations dans `src/`.

```text
include/ et src/
├── Core/       Fenêtre, scène, entité, caméra
├── Graphics/   Renderer, mesh, buffers, shader, texture, matériau,
│               framebuffer, post-traitement et ombres
├── Input/      Contrôleur caméra clavier/souris
├── Lighting/   Lumières ponctuelles, directionnelles et spots
├── Physics/    Corps rigides, monde physique et rendu de debug
├── Resources/  Chargement et cache des shaders, textures, matériaux, OBJ
└── Vendor/     Implémentation de stb_image

assets/
├── materials/
├── models/
├── shaders/
└── textures/
```

`Scene` possède ses entités et ses lumières. Une entité référence un mesh et un matériau, et porte un `Transform`. Le `Renderer` parcourt la scène et envoie les transformations, les matériaux et les lumières aux shaders.

## Fonctionnalités présentes

### Rendu

- OpenGL 3.3, VAO/VBO/EBO et description automatique des attributs de vertex.
- Position, normale, couleur et UV par vertex.
- Import de modèles OBJ, shaders GLSL chargés depuis des fichiers et textures chargées avec `stb_image`.
- `ShaderManager`, `TextureManager` et `MaterialManager` avec cache des ressources chargées.
- Matériaux configurables par fichier `.material` : texture, shininess, intensité spéculaire, opacité et paramètres d’émission.
- Éclairage Blinn-Phong : ambient, diffuse et spéculaire, avec lumières ponctuelles, directionnelles et spots. Le shader autorise jusqu’à 8 point lights, 4 directional lights et 8 spot lights.
- Normal mapping, emissive map, transparence simple, face culling, mode filaire et shadow map directionnelle.
- Framebuffer de post-traitement avec réglage d’exposition, tone mapping et correction gamma.

### Scène et caméra

- Entités avec ID, nom, activation/désactivation, recherche et suppression.
- Transformations locales : position, rotation Euler et échelle. La hiérarchie parent/enfant n’est pas encore implémentée.
- Caméras perspective avec FOV, plans near/far, yaw/pitch, sensibilité souris et vitesse configurables.
- Contrôleur permettant de créer plusieurs caméras et de changer de caméra.

### Physique actuelle

- Gravité, forces, impulsions, masse statique ou dynamique et intégration Euler semi-implicite.
- Collisions AABB/AABB, sphère/sphère et sphère/AABB.
- Correction de pénétration, restitution, friction et huit itérations de résolution par pas.
- Vitesse angulaire, inertie de base des boîtes et sphères, torque et impulsions de contact produisant une rotation.
- Le playground utilise un pas fixe de 1/120 seconde.

Exemple minimal d’utilisation :

```cpp
FuxEngine::PhysicsWorld physics;
auto& body = physics.CreateBody(
    cube,
    FuxEngine::CollisionShape::AABB,
    1.0f,
    glm::vec3(0.5f) // demi-dimensions locales
);
body.AddForce(glm::vec3(10.0f, 0.0f, 0.0f));
body.AddTorque(glm::vec3(0.0f, 0.0f, 1.0f));

// Dans la boucle de jeu, avec un pas fixe :
physics.Step(scene, fixedDeltaTime);
```

Une masse de `0.0f` rend le corps statique. Pour un collider sphérique, la dimension `x` représente le rayon local. Les forces et torques s’accumulent jusqu’au prochain pas physique.

## Lancer le playground

La scène de démonstration place plusieurs essais au-dessus d’une plateforme :

1. Un cube tombe sur le sol.
2. Deux cubes se rencontrent.
3. Deux sphères se rencontrent.
4. Un cube rebondit sur le sol.
5. Un cube glisse puis ralentit sous l’effet de la friction.

Contrôles :

- `W`, `A`, `S`, `D` : déplacer la caméra active.
- Souris : orienter la caméra.
- `1`, `2`, … : sélectionner une caméra disponible.
- `F` : basculer en rendu filaire.
- `F2` : afficher/masquer les colliders. Les AABB sont verts, les sphères cyan et les normales de contact rouges.
- `Échap` : fermer l’application.

## Compiler

Pré-requis : un compilateur C++20, CMake, une carte graphique compatible OpenGL 3.3 et un environnement graphique pour GLFW. CMake récupère GLFW et GLM via FetchContent ; le premier configure peut donc nécessiter un accès réseau.

```sh
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

Lancer ensuite `cmake-build-debug/FuxEngine` (ou `FuxEngine.exe` sous Windows). Les assets sont copiés à côté de l’exécutable après le build.

Il n’y a pas encore de suite de tests automatisés. Les collisions et le rendu physique se vérifient dans le playground.

## Limites connues

- Les boîtes de collision restent alignées aux axes même lorsque leur mesh tourne ; les collisions OBB ne sont pas implémentées.
- Les formes sphère et AABB sont prises en charge, mais pas les meshes de collision arbitraires ni les colliders composés.
- La détection teste toutes les paires de corps : elle devient coûteuse quand leur nombre augmente.
- Pas de détection continue : les objets rapides peuvent traverser une surface entre deux pas.
- La rotation utilise les angles Euler du `Transform` ; il n’y a pas encore de quaternion, d’amortissement angulaire ni de contraintes de joint.
- L’éclairage et la transparence sont simples : pas de PBR, HDR de production ou pipeline avancé de transparence.
- OBJ est le seul format de modèle importé. glTF, audio, système de ressources robuste et outils d’édition restent à faire.
- `Entity` référence actuellement son mesh et son matériau ; ces ressources doivent vivre aussi longtemps que les entités qui les utilisent.

## Pistes d’évolution

Ordre possible pour continuer le projet :

1. Ajouter des tests automatisés de physique et de chargement de ressources.
2. Améliorer le solveur : détection continue, broad phase, piles et stabilité numérique.
3. Ajouter les collisions de boîtes orientées pour que les colliders suivent la rotation visuelle.
4. Passer à une intégration angulaire par quaternion et compléter les impulses/torques appliqués hors centre.
5. Séparer le moteur de l’application de démonstration dans une bibliothèque réutilisable, puis introduire des systèmes et composants plus génériques.
6. Ajouter progressivement audio, glTF, outils d’inspection et autres systèmes selon les besoins.
