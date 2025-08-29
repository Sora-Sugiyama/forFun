//
//  main.cpp
//  tetris
//
//  Created by Sora Sugiyama on 8/30/25.
//

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <random>
#include "asset.h"

constexpr std::string_view BLANK="     ",FLOOR="🟧🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🧑🏽‍🌾🟧";
bool MAP[25][15];
void printCurrent(){
    std::cout<<"\033[2J\033[H";
    
    for(int i=1;i<=20;i++){
        std::cout<<BLANK<<"🍂";
        for(int j=1;j<=10;j++){
            if(MAP[i][j])std::cout<<"🥕";
            else std::cout<<"🍃";
        }
        std::cout<<"🍂\n";
    }
    std::cout<<BLANK<<FLOOR<<std::endl;
}

int dHeight[25];
int checkLine(){
    dHeight[21]=0;
    int cnt=0;
    for(int i=20;i>0;i--){
        dHeight[i]=dHeight[i+1];
        bool flag=true;
        for(int j=1;j<=10;j++){
            flag&=MAP[i][j];
        }
        if(flag){
            cnt++;
            dHeight[i]++;
        }
    }
    
    for(int i=20;i>0;i--){
        for(int j=1;j<=10;j++){
            if(i-dHeight[i]<=0)MAP[i][j]=false;
            else MAP[i][j]=MAP[i-dHeight[i]][j];
        }
    }
    return cnt;
}

std::random_device rd;

class block{
    u8 blockNum,x,y;
public:
    bool makeBlock(){
        blockNum=rd()%14;
        x=1;
        y=3;
        for(int i=x;i<x+blockSize[blockNum].first;i++){
            for(int j=y;j<y+blockSize[blockNum].second;j++){
                if(MAP[i][j]&&blocks[blockNum][i-x][j-y])return false;
            }
        }
        return true;
    }
    
    void printToMap(){
        for(int i=x;i<x+blockSize[blockNum].first;i++){
            for(int j=y;j<y+blockSize[blockNum].second;j++){
                MAP[i][j]|=blocks[blockNum][i-x][j-y];
            }
        }
    }
    
    void deleteFromMap(){
        for(int i=x;i<x+blockSize[blockNum].first;i++){
            for(int j=y;j<y+blockSize[blockNum].second;j++){
                if(blocks[blockNum][i-x][j-y])MAP[i][j]=false;
            }
        }
    }
    
    void rotateBlockLeft(){
        u8 tmp=rotateL[blockNum];
        for(int i=x;i<x+blockSize[tmp].first;i++){
            for(int j=y;j<y+blockSize[tmp].second;j++){
                if(MAP[i][j]&&blocks[tmp][i-x][j-y])return;
            }
        }
        blockNum=tmp;
    }
    
    void rotateBlockRight(){
        u8 tmp=rotateR[blockNum];
        for(int i=x;i<x+blockSize[tmp].first;i++){
            for(int j=y;j<y+blockSize[tmp].second;j++){
                if(MAP[i][j]&&blocks[tmp][i-x][j-y])return;
            }
        }
        blockNum=tmp;
        return;
    }
    
    bool moveDown(){
        for(int i=x;i<x+blockSize[blockNum].first;i++){
            for(int j=y;j<y+blockSize[blockNum].second;j++){
                if(MAP[i+1][j]&&blocks[blockNum][i-x][j-y])return false;
            }
        }
        x++;
        return true;
    }
    
    void moveRight(){
        for(int i=x;i<x+blockSize[blockNum].first;i++){
            for(int j=y;j<y+blockSize[blockNum].second;j++){
                if(MAP[i][j+1]&&blocks[blockNum][i-x][j-y])return;
            }
        }
        y++;
        return;
    }
    
    void moveLeft(){
        for(int i=x;i<x+blockSize[blockNum].first;i++){
            for(int j=y;j<y+blockSize[blockNum].second;j++){
                if(MAP[i][j-1]&&blocks[blockNum][i-x][j-y])return;
            }
        }
        y--;
        return;
    }
}BLOCK;

int main(int argc, const char * argv[]) {
    for(int i=0;i<=21;i++)MAP[i][0]=MAP[i][11]=true;
    for(int i=0;i<=11;i++)MAP[0][i]=MAP[21][i]=true;
    
    std::cin.tie(NULL);
    
    struct termios old_tio,new_tio;
    unsigned char c;
    
    tcgetattr(STDIN_FILENO,&old_tio);
    
    new_tio=old_tio;
    new_tio.c_lflag&=~(ICANON|ECHO);
    new_tio.c_cc[VMIN]=0;
    new_tio.c_cc[VTIME]=10;
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
    BLOCK.makeBlock();
    
    int numDeletedLines=0;
    while(1){
        BLOCK.printToMap();
        printCurrent();
        BLOCK.deleteFromMap();
        
        if(read(STDIN_FILENO,&c,1)==1){
            if(c=='\033'){
                unsigned char seq[2];
                if(read(STDIN_FILENO,&seq[0],1)==0)continue;
                if(read(STDIN_FILENO,&seq[1],1)==0)continue;
                if(seq[0]=='['){
                    switch(seq[1]){
                        case 'B':BLOCK.moveDown();break;
                        case 'C':BLOCK.moveRight();break;
                        case 'D':BLOCK.moveLeft();break;
                    }
                }
            }else if(c=='z'||c=='Z'){
                BLOCK.rotateBlockLeft();
            }else if(c=='x'||c=='X'){
                BLOCK.rotateBlockRight();
            }
        }
        
        if(!BLOCK.moveDown()){
            BLOCK.printToMap();
            numDeletedLines+=checkLine();
            if(!BLOCK.makeBlock()){
                std::cout<<"🔥 GAME OVER 🔥\n🚜 Number of Delete Lines: "<<numDeletedLines<<std::endl;
                break;
            }
        }
    }
    
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
    return 0;
}
