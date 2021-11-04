#include <ctime>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#define FILE_NAME "textFile.txt"

struct myMSG {
    long mtype; // long because this is a must (this is a type; response ? progNum+10 : progNum)
    int progNum;
    int queueID;
    time_t sendTime;
};
typedef struct myMSG myMSG;

struct reqMSG {
    long progNum; // long because this is a must
    int queueID;
    time_t reqTime;
};

struct resMSG {
    long progNum; // long because this is a must
    time_t resTime;
};

void readFile() {
    FILE *fp;
    if (fp = fopen(FILE_NAME, "rt")) {
        fseek(fp, 0, SEEK_END); // to the EOF

        long fileSize = ftell(fp); // get file size
        fseek(fp, 0, SEEK_SET); 

        fread(cout, 1, fileSize, fp);
        
        fclose(fp);
    } else cout << endl << "Error opening file";
}

void resSend(int progNum, int queueID, int sendTo) {
    time_t resTime = time(NULL); // time in seconds
    myMSG res = {progNum+10, progNum, queueID, resTime};
    int send = msgsnd(queueID, &res, sizeof(myMSG), 0); // send request to read file
    //cout << "Response to program " << sendTo << " was sent. Time: " << resTime << endl;
}