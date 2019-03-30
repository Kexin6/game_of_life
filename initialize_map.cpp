void initialize_map (bool iMap[32][24], bool nMap[32][24]){
    int i, j;
    for (i = 0; i < 32; i++){
        for (j = 0; j < 24; j++){
         	  initialMap[i][j] = rand() % 2; //should change to srand() later
            nextMap[i][j] = initialMap[i][j];
        }
    }
}
