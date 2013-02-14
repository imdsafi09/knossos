//commented out thread loop
#include "knossos-global.h"
#include "remote.h"
#include "sleeper.h"

extern stateInfo *state;

Remote::Remote(QObject *parent) :
    QObject(parent)
{
}

static bool cleanUpRemote() {
    free(state->remoteState);
    state->remoteState = NULL;

    return true;
}

void Remote::start() {
    struct remoteState *remoteState = state->remoteState;

    // remoteSignal is != false as long as the remote is active.
    // Checking for remoteSignal is therefore a way of seeing if the remote
    // is available for doing something.
    //
    // Depending on the contents of remoteState, this thread will either go
    // on to listen to a socket and get its instructions from there or it
    // will follow the trajectory given in a file.
    int i = 0;
    while(true) {
        //qDebug("remote says hello %i", ++i);
        Sleeper::msleep(50);
        /*SDL_LockMutex(state->protectRemoteSignal);
        while(state->remoteSignal == false) {
            SDL_CondWait(state->conditionRemoteSignal, state->protectRemoteSignal);
        }

        state->remoteSignal = false;
        SDL_UnlockMutex(state->protectRemoteSignal);

        if(state->quitSignal == true)
            break;

        updateRemoteState();

        switch(remoteState->type) {
            case REMOTE_TRAJECTORY:
                remoteTrajectory(remoteState->activeTrajectory);
                break;

            case REMOTE_RECENTERING:
                //remoteRecentering();
                remoteWalkTo(state->remoteState->recenteringPosition.x, state->remoteState->recenteringPosition.y, state->remoteState->recenteringPosition.z);
                break;

            default:
                printf("No such remote type (%d)\n", remoteState->type);
        }

        if(state->quitSignal == true) {
            break;
        }
        */
    }

    cleanUpRemote();

    QThread::currentThread()->quit();
    emit finished();
}

void Remote::checkIdleTime() { }

int32_t Remote::remoteJump(int32_t x, int32_t y, int32_t z) { return 0;}
