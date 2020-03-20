#include <queue>

typedef enum {data,ack,nak} frame_kind;
typedef struct frame_head
{
frame_kind kind; //帧类型
unsigned int seq; //序列号
unsigned int ack; //确认号
unsigned char data[100];//数据
};
typedef struct frame
{
frame_head head; //帧头
unsigned int size; //数据的大小
};

queue<frame> frames_to_send;
deque<frame> frames_sent;

int stud_slide_window_stop_and_wait(char *pBuffer, int bufferSize, UINT8 messageType){

    switch(messageType){
        case MSG_TYPE_TIMEOUT:{
            frame f = frames_sent.front();
            SendFRAMEPacket((unsigned char *)&f, f.size);
            break;
        }
        case MSG_TYPE_SEND:{
            frame f;
            memcpy(&f, pBuffer, sizeof(frame));
            f.size = bufferSize;
            frames_to_send.push(f);
            if(frames_sent.empty()){ // can send more frames!
                f = frames_to_send.front();
                frames_to_send.pop();
                frames_sent.push_back(f);
                SendFRAMEPacket((unsigned char *)&f, f.size);
            }
            break;
        }
        case MSG_TYPE_RECEIVE:{
            frames_sent.pop_front();
            if(frames_to_send.size()){
                frame f = frames_to_send.front();
                frames_to_send.pop();
                frames_sent.push_back(f);
                SendFRAMEPacket((unsigned char *)&f, f.size);
            }
            break;
        }
        default:{
            return -1;
        }
    }
    return 0;
}

int stud_slide_window_back_n_frame(char *pBuffer, int bufferSize, UINT8 messageType){

    switch(messageType){
        case MSG_TYPE_TIMEOUT:{
            for(deque<struct frame>::iterator it = frames_sent.begin();
                it != frames_sent.end();
                it++){
                    SendFRAMEPacket((unsigned char *)&(*it), (*it).size);
            }
            break;
        }
        case MSG_TYPE_SEND:{
            frame f;
            memcpy(&f, pBuffer, sizeof(frame));
            f.size = bufferSize;
            frames_to_send.push(f);
            while(frames_to_send.size() && frames_sent.size() < n){ // can send more frames...but what is n?
                f = frames_to_send.front();
                frames_to_send.pop();
                frames_sent.push_back(f);
                SendFRAMEPacket((unsigned char *)&f, f.size);
            }
            break;
        }
        case MSG_TYPE_RECEIVE:{
            frame f;
            memcpy(&f, pBuffer, sizeof(frame));
            while(frames_sent.front().head.seq != f.head.ack && frames_sent.size()){
                frames_sent.pop_front();
            }
            frames_sent.pop_front();
            while(frames_to_send.size() && frames_sent.size() < n){
                f = frames_to_send.front();
                frames_to_send.pop();
                frames_sent.push_back(f);
                SendFRAMEPacket((unsigned char *)&f, f.size);
            }
        }
        default:{
            return -1;
        }
    }
    return 0;
}

int stud_slide_window_choice_frame_resend(char *pBuffer, int bufferSize, UINT8 messageType){

    switch(messageType){
        case MSG_TYPE_TIMEOUT:{
            unsigned int seq; //序列号
            memcpy(&seq, pBuffer, sizeof(seq));
            for(deque<struct frame>::iterator it = frames_sent.begin();
                it != frames_sent.end();
                it++){
                if(seq == (*it).head.seq){
                    SendFRAMEPacket((unsigned char *)&(*it), (*it).size);
                    break;
                }
            }
            break;
        }
        case MSG_TYPE_SEND:{
            frame f;
            memcpy(&f, pBuffer, sizeof(frame));
            f.size = bufferSize;
            frames_to_send.push(f);
            while(frames_to_send.size() && frames_sent.size() < n){ // can send more frames...but what is n?
                f = frames_to_send.front();
                frames_to_send.pop();
                frames_sent.push_back(f);
                SendFRAMEPacket((unsigned char *)&f, f.size);
            }
            break;
        }
        case MSG_TYPE_RECEIVE:{
            frame f;
            memcpy(&f, pBuffer, sizeof(frame));

            if(ntohl(f.head.kind) == ack){
                // delete acknowledged frames
                while(frames_sent.front().head.seq != f.head.ack && frames_sent.size()){
                    frames_sent.pop_front();
                }
                frames_sent.pop_front();
                // send new ones
                while(frames_to_send.size() && frames_sent.size() < n){
                    f = frames_to_send.front();
                    frames_to_send.pop();
                    frames_sent.push_back(f);
                    SendFRAMEPacket((unsigned char *)&f, f.size);
                }
            }
            if(ntohl(f.head.kind) == nak){
                for(deque<struct frame>::iterator it = frames_sent.begin();
                    it != frames_sent.end();
                    it++){
                    if(f.head.ack == (*it).head.seq){
                        SendFRAMEPacket((unsigned char *)&(*it), (*it).size);
                        break;
                    }
                }
            }

        }
        default:{
            return -1;
        }
    }
    return 0;
}
