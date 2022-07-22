# Usage


## Dependencies

This project use `ffmpeg-io` and `nrc2` project as submodules:

```bash
git submodule update --init --recursive
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
 * `-DTAH_DETECT_EXE` [default=`ON`] {possible:`ON`,`OFF`}: compile the detection chain executable.
 * `-DTAH_VISU_EXE` [default=`ON`] {possible:`ON`,`OFF`}: compile the visual tracking executable.
 * `-DTAH_CHECK_EXE` [default=`ON`] {possible:`ON`,`OFF`}: compile the check executable.

## **Détection**

Exécutable de la chaine de detection de méteores dans `./exe/meteor-detect`. \
**Input** `input-video` : video sur laquelle on veut détecter des météores \
**Output** : `tracks.txt` avec les météores "détectés", par défaut dans "./debug/"\  
\
Pour les options : 

` -input_video   ` : Video source (str)  \
` -output_tracks ` : Path frames output (str)  \
` -output_stats  ` : Save files in output_path (str)  \
` -start_frame   ` : Image de départ dans la séquence (int)  \
` -end_frame     ` : Dernière image de la séquence (int)  \
` -skip_frames   ` : Nombre d'images à sauter (int)  \
` -light_min     ` : Seuil bas filtrage lumineux (int)  \
` -light_max     ` : Seuil haut filtrage lumineux (int)  \
` -surface_min   ` : Surface max des CC en pixels (int)  \
` -surface_max   ` : Surface min des CC en pixels (int)  \
` -k             ` : Nombre de voisins dans KPPV (int)  \
` -r_extrapol    ` : Rayon de recherche d'une CC dans le cas d'une extrapolation (int)  \
` -d_line        ` : Delta pour lequel un point est toujours considéré comme étant sur une droite (int)  \
` -diff_deviaton ` : Facteur de multiplication de l'ecart type (l'erreur d'une CC doit etre superieure a diff_deviation*ecart_type pour etre considéré en mouvement (float)  \

## **Visualisation**

Exécutable de la visualisation de la détection des méteores dans `./exe/meteor-visu`. \
**Input** ` -input_tracks `:  Video sur laquelle on veut détecter des météores \
**Input** ` -input_video  `:  Ficher tracks.txt correspondant à la video input \
**Input2** (optionnelle) ` -validation   ` : Fichier Vérité Terrain \
**Output** ` -output_video `: Si Inputs2, alors vidéo avec rectangles englobants en couleur (Vert = Vrai positif / Rouge = Faux positif) \
sinon vidéo avec rectangles englobants en plusieurs teintes de vert selon le niveau de confiance de la détection.  

Pour les options : 

` -input_tracks ` : `tracks.txt` \
` -input_video  ` : Video source \
` -output_video ` : Path video output \
` -validation   ` : Fichier contenant la vérité terrain de la séquence pour mettre les couleurs (Rouge = faux positif / Vert = vrai positif) \

Pour exécuter `./exe/meteor-visu`, il faut impérativement avoir lancer `./exe/meteor-detect` sur la même vidéo auparavant pour avoir les fichiers `tracks.txt` et `bouding_box.txt`.

## **Validation**

Exécutable de la vérification de la détection des méteores sous format texte dans `./exe/meteor-check`. \
**Input** ` -input_tracks ` :  video sur laquelle on veut détecter des météores \
**Input**  ` -validation   ` : Fichier contenant la vérité terrain de la séquence \
**Output** ` -output` : path du dossier contenant `validation.txt`, par défaut dans le dossier courant\

Pour les options : 

` -input_tracks `: `tracks.txt` \
` -output       `: path du dossier contenant `validation.txt`, par défaut dans le dossier courant\ \
` -validation   `: Fichier contenant la vérité terrain de la séquence \

Pour exécuter `./exe/meteor-check`, il faut impérativement avoir lancer `./exe/meteor-detect` sur la même vidéo auparavant pour avoir le fichier `tracks.txt`.


## Exemple : 

Téléchargez la vidéo dans la racine du project: https://lip6.fr/Adrien.Cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4

### Step 1 : Détection de météores

```shell
./exe/meteor-detect -input_video ../2022_05_31_tauh_34_meteors.mp4
```

Pour avoir les frames en binaire pour debug : 

```shell
./exe/meteor-detect -input_video ../2022_05_31_tauh_34_meteors.mp4 -output_frames ./frames
```

### Step 2 : Visualisation de la détection

```shell
./exe/meteor-visu -input_video ../2022_05_31_tauh_34_meteors.mp4 -input_tracks ./debug/assoconflicts/SB_55_SH_80/2022_05_31_tauh_34_meteors/tracks.txt -output ../.
```

Pour avoir la visualisation avec la vérité Terrain :

```shell
./exe/meteor-visu -input_video ../2022_05_31_tauh_34_meteors.mp4 -input_tracks ./debug/assoconflicts/SB_55_SH_80/2022_05_31_tauh_34_meteors/tracks.txt -output ../ -validation ../validation/meteor24.txt
```

### Step 3 : Validation par fichier texte

Rajoutez l'option suivante:
```shell
./exe/meteor-check -input_tracks ./debug/assoconflicts/SB_55_SH_80/meteor24/tracks.txt -validation ../validation/meteor24.txt
```

