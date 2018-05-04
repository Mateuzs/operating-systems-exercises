//
//  messages.h
//  lab6
//
//  Created by Mateusz Zembol on 23.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#ifndef messages_h
#define messages_h


#define maxOrderValueLen 32

#define keyGen 'K'

#define maxClients 32

struct Order{
    long type;
    char value[maxOrderValueLen];
    int qid;
    int pid;
};

enum TaskNumber{
    NEWCLIENT=1,MIRROR,CALC,TIME,END, STOP
};

#endif /* messages_h */
