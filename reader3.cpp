#include <iostream>
#include <sys/msg.h>
#include <sys/types.h>
#include <ctime>
#include <stdio.h>
#include "datatypes.h"
#include <errno.h>
#include <sys/ipc.h>

const int NAME = 3;

using namespace std;

int main() {
    key_t mainKey, perKey;
    int mainQueue, perQueue;

    mainKey = ftok(FILE_NAME, 8);
    perKey = ftok(FILE_NAME, 3);
    cout << mainKey << endl;
    cout << perKey << endl;

    mainQueue = msgget(mainKey, IPC_CREAT | IPC_EXCL | 0606); // group queue; 0040 = reading for group; 0020 = writing for group
    if (mainQueue == -1) {
        mainQueue = msgget(mainKey, IPC_CREAT);
        if (mainQueue != -1)
            cout << "Common queue created successfully." << endl;
        else
            cout << "Error creating common queue " << errno << endl;
    } else
        cout << "Common queue created successfully." << endl;


    perQueue = msgget(perKey, IPC_CREAT | IPC_EXCL | 0606); // personal queue; 0400 = reading for author; 0002 = writing for other
    if (perQueue == -1) {
        perQueue = msgget(perKey, IPC_CREAT);
        if (perQueue != -1)
            cout << "Personal queue created successfully." << endl;
        else
            cout << "Error creating personal queue " << errno << endl;
    } else
        cout << "Personal queue created successfully." << endl;

    bool read = false;
    int others = 0;
    time_t reqTime = time(NULL); // time in seconds
    myMSG req = {8, NAME, perQueue, reqTime};

    int send = msgsnd(mainQueue, &req, sizeof(myMSG)-sizeof(long), 0); // send request to read file
    if (send == -1)
        cout << "Shit! " << errno <<  endl;
    else {
    
    cout << endl << "Sent request to read file... Time: " << reqTime << endl;
    cout << endl << "Waiting for responses...";

    while (read == false || others < 3) {
        myMSG getMsg;
        int recMsg = msgrcv(mainQueue, &getMsg, sizeof(myMSG), 8, 0);
        if (getMsg.progNum != NAME) {
            resSend(3, getMsg.queueID, getMsg.progNum); // send response to getMsg.queueID
            others++;
        } else {
            // getResponse
            myMSG getMsg1, getMsg2;
            int resMsg1 = msgrcv(perQueue, &getMsg1, sizeof(myMSG), 11, 0);
            if (resMsg1 != -1) 
                cout << "Received response from program " << getMsg1.progNum << "; Time: " << getMsg1.sendTime << endl; 
            int resMsg2 = msgrcv(perQueue, &getMsg2, sizeof(myMSG), 12, 0);
            if (resMsg2 != -1) 
                cout << "Received response from program " << getMsg2.progNum << "; Time: " << getMsg2.sendTime << endl;
            
            // read
            if (resMsg1 != -1 && resMsg2 != -1) {
                readFile();
                read = true;
            } else 
                cout << endl << "Error receiving response.";
        }
    }

    }
    return 0;
}