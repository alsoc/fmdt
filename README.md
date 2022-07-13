# Usage

Changer paths des outputs en fonction de votre espace de stockage dans DebugUtil.c (surtout SEQUENCE_DST_PATH_VIDEOS)

## Dependencies

This project use `ffmpeg-io` project as a submodule:

```bash
git submodule update --init --recursive
```

## Make

Pour la création des binaires `ballon` et `tracking` :

```shell
make
```

## CMake

```bash
mkdir build
cd build
cmake ..
make -j4
```

Example of optimization flags:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-Wall -funroll-loops -fstrict-aliasing -march=native"
```

**Tips**: on Apple Silicon M1 CPUs and with Apple Clang, use `-mcpu=apple-m1` instead of `-march=native`.

The `CMake` file comes with several options:
 * `-DTH_BALLON_EXE` [default=`ON`] {possible:`ON`,`OFF`}: compile the detection chain executable.
 * `-DTH_TRACKING_EXE` [default=`ON`] {possible:`ON`,`OFF`}: compile the tracking executable.

## Chaine de traitement

Exécution de la chaine de traitement d'images dans `./ballon`. \
Input :  video sur laquelle on veut détecter des météores \
Input2 (optionnelle) : fichier Vérité Terrain \
Output : fichier tracks.txt avec les météores "détectés" (Vrai et Faux positifs)\  
Output (friendly user): fichier bounding_box.txt pour les rectangles englobants dans `./tracking`   
Output2 (optionnelle) : si Input2, alors les résultats des météores détectés par rapport aux Vérités terrains dans /debug/validation.txt \
\
Pour les options : 

`-input`       : Video input où il faut détecter les météores (str)\
`-start_frame` : Image de départ de l'input (int)\
`-end_frame  ` : Image de fin  de l'input (int)\
`-light_min  ` : Seuil bas du seuillage par hystérésis (int)\
`-light_max  ` : Seuil haut du seuillage par hystérésis (int)\
`-surface_min` : Surface min des CC en pixels (int)\
`-surface_max` : Surface max des CC en pixels (int)\
`-debug`       : Enregistre des données pour debugger dans debug/ (1/0) \
`-validation ` : Fichier contenant la vérité terrain de la séquence (str)

## Videos

Obtention des vidéos outputs dans `./tracking` \
Input :  video sur laquelle on veut détecter des météores \
Input :  ficher tracks.txt correspondant à la video input \
Input2 (optionnelle) : fichier Vérité Terrain \
Output : si Inputs2, alors vidéo avec rectangles englobants en couleur (Vert = Vrai positif / Rouge = Faux positif) \
sinon vidéo avec rectangles englobants en orange (pas de distinction entre vrai et faux positif)

Pour les options : 

`-input_video` : Video input où il faut détecter les météores (str)\
`-input_tracks`: Tracks des météores (str)\
`-start_frame` : Image de départ de l'input (int)\
`-end_frame  ` : Image de fin  de l'input (int)\
`-light_min  ` : Seuil bas du seuillage par hystérésis (int)\
`-light_max  ` : Seuil haut du seuillage par hystérésis (int)\
`-surface_min` : Surface min des CC en pixels (int)\
`-surface_max` : Surface max des CC en pixels (int)\
`-validation ` : Fichier contenant la vérité terrain de la séquence (str)
`-filename `   : nom de la video (c'est temporaire, on fait ca rapidement) (str)

Pour exécuter `./traking`, il faut impérativement avoir lancer `./ballon` sur la même vidéo auparavant pour avoir le fichier tracks.txt et bouding_box.txt


## Exemple sur le réseau du LIP6 : 



### Exemple 1 : récupérer les météores détectés sous format texte dans debug/assoconflicts/

```shell
./ballon -input /users/rodata/meteorix/video/2022-TAH/2022-TAH/Videos/mkv/Basler_1920/Basler_acA1920-155um__23278758__20220531_042259204.mkv -start_frame 1 -end_frame 5000 -light_min 60 -light_max 85 -surface_min 9 -surface_max 1000 -debug 0 
```

Rajoutez l'option suivante pour comparer les résultats avec la vérité terrain établie à la main:
```shell
-validation ./validation/Basler_acA1920-155um__23278758__20220531_042259204.txt
```

Les résultats sont enregistrés dans /debug/validation.txt

### Exemple 2 : récupérer une video des "météores détectés" avec rectangle englobant sans distinction Vrai/Faux positifs

```shell
./tracking -start_frame 0 -end_frame 5000 -light_min 60 -light_max 85 -input_video /users/rodata/meteorix/video/2022-TAH/2022-TAH/Videos/mkv/Basler_1920/Basler_acA1920-155um__23278758__20220531_042259204.mkv -input_tracks ./debug/assoconflicts/SB_60_SH_85/Basler_acA1920-155um__23278758__20220531_042259204/tracks.txt
 ```

### Exemple 3 : récupérer une video des "météores détectés" avec rectangle englobant avec Vrai/Faux positifs

Rajoutez l'option suivante:
```shell
-validation ./validation/Basler_acA1920-155um__23278758__20220531_042259204.txt
```