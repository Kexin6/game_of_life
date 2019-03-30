extern bool check_survival(bool map[32][24], unsigned i, unsigned j, unsigned survial){
	unsigned count = 0;
    //check if is boundary
    /* case 1: corner */
    if (i == 0 && j == 0){
    	if (map[0][1] == 1) count++;
        if (map[1][0] == 1) count++;
        if (map[1][1] == 1) count++;
    }
    else if (i == 0 && j == 23){ //left bottom
        if (map[0][22] == 1) count++;
        if (map[1][23] == 1) count++;
    	if (map[1][22] == 1) count++;
    }
    else if (i == 31 && j == 0){
    	if (map[30][0] == 1) count++;
        if (map[31][1] == 1) count++;
        if (map[30][1] == 1) count++;
    }
    else if (i == 31 && j == 23){
    	if (map[31][22] == 1) count++;
        if (map[30][23] == 1) count++;
        if (map[30][22] == 1) count++;
    }
    /* case 2: edge */
    else if (i == 0){
    	if (map[0][j-1] == 1) count++;
        if (map[0][j+1] == 1) count++;
        if (map[1][j-1] == 1) count++;
        if (map[1][j] == 1) count++;
        if (map[1][j+1] == 1) count++;
    }
    else if (i == 31){
        if (map[31][j-1] == 1) count++;
        if (map[31][j+1] == 1) count++;
        if (map[30][j-1] == 1) count++;
        if (map[30][j] == 1) count++;
        if (map[30][j+1] == 1) count++;
    }
    else if (j == 0){
        if (map[i-1][0] == 1) count++;
        if (map[i+1][0] == 1) count++;
        if (map[i-1][1] == 1) count++;
        if (map[i][1] == 1) count++;
        if (map[i+1][1] == 1) count++;
    }
    else if (j == 23){
        if (map[i-1][23] == 1) count++;
        if (map[i+1][23] == 1) count++;
        if (map[i-1][22] == 1) count++;
        if (map[i][22] == 1) count++;
        if (map[i+1][22] == 1) count++;
    }    
    /* case 3: general */
    else{
    	if (map[i-1][j-1] == 1) count++;
        if (map[i-1][j] == 1) count++;
        if (map[i][j-1] == 1) count++;
        if (map[i+1][j+1] == 1) count++;
        if (map[i+1][j] == 1) count++;
        if (map[i][j+1] == 1) count++;
        if (map[i+1][j-1] == 1) count++;
        if (map[i-1][j+1] == 1) count++;
    }
    if (count == survial) return 1;
    else return 0;
}
