#include "settings.h"
#include <QFile>
#include <QSettings>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "helpers.h"
Settings S;
ParticipantData ppData;
ParticipantControl ppControl;

QString Settings::conf_baseInstructions[][3] =
{
    { "setup",
      "spaceholder",
      "spaceholder"
    },
    { "overview",
      "<h2>\
                 Overview\
              </h2>\
              <div style = \"height: 400px; width: 1200px;        line-height: 1.25;\">\
                   <p> In this experiment, you will see visualizations on the desktop and in VR. <br> \
                    You will first finish a question session, followed by four main sessions. There will be another question session at the end. <br>\
                    All the sessions together take about 60 - 90 minutes.  You will have breaks between the sessions.\
                    </p>\
                    <p>\
                       <br>You will see detailed instructions later. If you have any questions, free feel to ask the experimenter.\
                    </p>\
                    <p>\
                       <br>Please do not close or resize this window. Please do not check your phone in the experiment.\
                    </p>\
              </div>",
      "overview 1"
    },

    { "prequestions",
      "<h2>\
             Pre-Questions\
             </h2>\
             <div style = \"height: 10px; width: 1200px;\">\
             </div>",
      "prequestions"
    },

    { "taskIntroduction1",
      "<h2>\
               Your Tasks (Important)\
              </h2>\
              <div style = \"height: 200px; width: 1200px;        line-height: 1.25;\">\
                   <p>\
                      You will see scatterplots on the screen or in VR. \
                      The scatterplots show <strong>the outputs of machine learning classifiers</strong> for test datasets.\
                   <br>\
                      Each dot represents an instance. \
                      <strong>The color of a dot represents the ground truth class of that instance.</strong> \
                   <br>\
                      The same color represents the same class. Different colors represent different classes. \
                      Assume that the same color in different scatterplots represents different classes.\
                      For each scatterplot, we ask you to <strong> estimate the classification accuracy </strong> for that dataset. \
                   <br>\
                      Please see some examples below. There are different scatterplots.\
                  </p>\
                 </div>",
      "<div style = \"height: 50px; width: 1200px;        line-height: 1.25;\">\
               <p>\
                You see <strong>one scatterplot each time</strong>. Then, you proceed to provide your estimation of accuracy, and <b>the scatterplot will be removed</b>.\
                You will have <strong> a few practice trials with feedback </strong> to decide how you are going to estimate accuracy.\
                There is no feedback in the main trials.\
               </p>\
               <p style = \"font-size: 14pt; margin-top: 20px;\"> <i>P.S., If you want to know more about the dataset used in a scatterplot, <br>\
                  the dataset is the output of the last hidden layer of a neural network, \
                  and the scatterplot shows a t-SNE projection of the output in a lower-dimensional (2 or 3) space.</i></p>\
            </div>"
    },

    { "taskIntroduction2",
      "<h2>\
               Your Tasks (con't)\
              </h2>\
              <div style = \"height: 400px; width: 1200px;        line-height: 1.25;\">\
                   <p>\
                      There are <strong>four estimation sessions</strong>: \
                      two on the desktop and the other two in VR.\
                   </p>\
                   <p>\
                      <br>In some sessions, you see <b>static pictures</b>. \
                   </p>\
                   <p>\
                     In other sessions, you can <b>interact with the scatterplots</b>.\
                     <br>You press the <b>left mouse button</b> or the <b>touch pad</b> of the controller to <b>rotate</b> the scatterplot.\
                     You use the <b>WASD</b> keys or <b>walk in VR</b> to <b>navigate</b>.\
                  </p>\
                  <p>\
                     <br>You use the <b>space key</b> or the <b>trigger</b> of the controller to go to the <b>next step</b>. \
                  </p>\
                 </div>",
      "<div style = \"height: 50px; width: 1200px;        line-height: 1.25;\">\
                <p>\
                   <br>When you are ready, click the next button to proceed to the first session.\
                </p>\
             </div>"
    },

    { "session1",
      "<h2> Session 1 : Introduction </h2>",
      "session 1"
    },

    { "practice1",
        //      "<h2> Session 1 : Practice Trials </h2>",
      "Practice Trials \nNo time constraint in the first three trials",
      "practice 1"
    },

    { "interval1",
        //      "<h2> Session 1 : Practice Finished </h2>",
      "Practice trials end",
      "interval 1"
    },

    { "main1",
        //      "<h2> Session 1 : Main Trials </h2>",
      "Main trials start right now\nNo feedback will be given",
      "main 1"
    },

    { "endtrials1",
        //      "<h2> Session 1 : End Trials </h2>",
      "Trials for this session end",
      "end 1"
    },

    { "openquestions1",
      " <div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
             <h2> Session 1: Open-ended questions</h2> \
             <p> Please answer the following questions. </p>\
        </div>",
      "openquestions1-spaceholder"
    },

    { "break1",
      "<div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
             <h2> Session 1 : Take a Break </h2> \
             <p> Please take a short break. You will be able to proceed to <b>Session 2</b> after</p>\
          </div>",
      "break1-spaceholder "
    },

    { "session2",
      "<h2> Session 2 : Introduction </h2>",
      "session2-spaceholder"
    },

    { "practice2",
        //      "<h2> Session 2 : Practice Trials </h2>",
      "Practice Trials\nNo time constraint in the first three trials",
      "practice 2"
    },

    { "interval2",
        //      "<h2> Session 2 : Practice Finished </h2>",
      "Practice trials end",
      "interval 2 end"
    },

    { "main2",
        //      "<h2> Session 2 : Main Trials </h2>",
      "Main trials start right now\nNo feedback will be given",
      "main 2"
    },

    { "endtrials2",
        //      "<h2> Session 2 : End Trials </h2>",
      "Trials for this session end",
      "end 2"
    },

    { "openquestions2",
      "<div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
                             <h2> Session 2 : Open-ended questions</h2> \
                                 <p> Please answer the following questions. </p>\
                             </div>",
      "openquestions2-spaceholder"
    },

    { "break2",
      " <div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
                             <h2> Session 2 : Take a Break </h2> \
                             <p> Please take a short break. You will be able to proceed to <b>Session 3</b> after</p>\
                             </div>",
      "spaceholder"
    },

    { "session3",
      "<h2> Session 3 : Introduction </h2>",
      "session3-spaceholder"
    },

    { "practice3",
        //      "<h2> Session 3 : Practice Trials </h2>",
      "Practice Trials\nNo time constraint in the first three trials",
      "practice 3"
    },

    { "interval3",
        //      "<h2> Session 3 : Practice Finished </h2>",
      "Practice trials end",
      "interval 3"
    },

    { "main3",
        //      "<h2> Session 3 : Main Trials</h2>",
      "Main trials start right now\nNo feedback will be given",
      "main 3"
    },

    { "endtrials3",
        //      "<h2> Session 3 : End Trials </h2>",
      "Trials for this session end",
      "end 3"
    },

    { "openquestions3",
      " <div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
                             <h2> Session 3 : Open-ended questions</h2> \
                                 <p> Please answer the following questions. </p>\
                             </div>",
      "openquestions3-spaceholder"
    },

    { "break3",
      " <div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
                             <h2> Session 3 : Take a Break </h2> \
                             <p> Please take a short break. You will be able to proceed to <b>Session 4</b> after</p>\
                             </div>",
      "break3-spaceholder"
    },

    { "session4",
      "<h2> Session 4 : Introduction </h2>",
      "session4-spaceholder"
    },

    { "practice4",
        //      "<h2> Session 4 : Practice Trials </h2>",
      "Practice Trials\nNo time constraint in the first three trials",
      "practice 4"
    },

    { "interval4",
        //      "<h2> Session 4 : Practice Finished </h2>",
      "Practice trials end",
      "interval 4"
    },

    { "main4",
        //      "<h2> Session 4 : Main Trials</h2>",
      "Main trials start right now\nNo feedback will be given",
      "main 4"
    },

    { "endtrials4",
        //      "<h2> Session 4 : End Trials </h2>",
      "Trials for this session end",
      "end 4"
    },

    { "openquestions4",
      " <div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
                             <h2> Session 4 : Open-ended questions</h2> \
                                 <p> Please answer the following questions. </p>\
                             </div>",
      "openquestions4-spaceholder"
    },

    //   { "break4",           "<h2> Session 4 : Break </h2>",                "spaceholder"                                                                        },

    { "post",
       "<div style = \"height: 50px; width: 1200px;        line-height: 1.25\">\
                            <h2> Last Session: Post-experiment Questions </h2> \
                                <p> Please answer the following questions. </p>\
                            </div>",
      "post-spaceholder"
    },

    { "end",
      "<h2> End </h2>",
      "end-spaceholder"
    }
};


const QString Settings::conf_sessionsIntroductions[][4] =
{
    { "stereoscopy: no; motion: no",
      " <div style = \"height: 100px; width: 1200px;        line-height: 1.25\">\
        <p>\
            In this session, you finish all the trials <b>on the desktop</b>. \
            You will see <b>static scatterplots</b> on the desktop. You <b>can't interact</b> with them.\
            <br> You provide your estimation of accuracy by pressing different areas on the screen.\
        </p>\
        <p>\
            <br>No time constraint in the <b>first three</b> practice trials. You can explore the scatterplots as long as you want.<br>\
            In each of the rest trials, you have <b> up to 1 minute</b>.\
            After 1 minute, the scatterplot will be removed.\
            There is no time limit on providing your estimation.\
        </p>\
        <p>\
            <br>Click the next button to proceed to the first trial. Ask the experimenter if you have any question.\
        </p>\
      </div>",
      ":/resources/no-no.png",
      "" },

    { "stereoscopy: no; motion: yes",
      " <div style = \"height: 100px; width: 1200px;        line-height: 1.25\">\
        <p>\
            In this session, you finish all the trials <b>on the desktop</b>. \
            <br>You can use the <b>mouse and keyboard</b> to <b>rotate</b> the scatterplots and <b>change your perspective</b>.<br>\
            You provide your estimation of accuracy by pressing different areas on the screen.\
        </p>\
        <p>\
            <br>No time constraint in the <b>first three</b> practice trials. You can explore the scatterplots as long as you want.<br>\
            In each of the rest trials, you have <b> up to 1 minute</b>.\
            After 1 minute, the scatterplot will be removed.\
            There is no time limit on providing your estimation.\
        </p>\
        <p>\
            <br>Click the next button to get started.\
        </p>\
      </div>",
      ":/resources/no-yes.png",
      ""
    },

    { "stereoscopy: yes; motion: no",
      " <div style = \"height: 100px; width: 1200px;        line-height: 1.25\">\
        <p>\
            In this session, you finish all the trials <b> in VR</b> using the headset. \
            You will see <b>static scatterplots</b> in VR. You <b>can't interact</b> with them. <br>\
            You provide your estimation of accuracy by holding the controller in a higher/lower position and pressing the touch pad.\
        </p>\
        <p>\
            <br>You <b>sit in the chair</b> and <b>face the diagonal corner</b> of this room for this VR session. \
        </p>\
        <p>\
            <br>No time constraint in the <b>first three</b> practice trials. You can explore the scatterplots as long as you want.<br>\
            In each of the rest trials, you have <b> up to 1 minute</b>.\
            After 1 minute, the scatterplot will be removed.\
            There is no time limit on providing your estimation.\
        </p>\
        <p>\
            <br>Click the next button to launch VR. Ask the experimenter to help you wear the headset.\
        </p>\
      </div>",
      ":/resources/yes-no.png",
      "" },
    { "stereoscopy: yes; motion: yes",
      " <div style = \"height: 100px; width: 1200px;        line-height: 1.25\">\
        <p>\
            In this session, you finish all the trials <b> in VR</b> using the headset. <br>\
            You see the scatterplot differently <b>while you are moving</b>; you can <b>press and drag</b> the controller to rotate the scatterplot.<br>\
            You provide your estimation of accuracy by holding the controller in a higher/lower position and pressing the touch pad.\
        </p>\
        <p>\
            <br> You <b>stand up and walk in the room</b> for this VR session.</b> \
            You start with a <b>home position</b> (a gray triangle on the floor in VR, where you are sitting). If you moved in a trial, please <b>come back</b> to the home position <b>as cued</b>. \
        </p>\
        <p>\
             <br>No time constraint in the <b>first three</b> practice trials. You can explore the scatterplots as long as you want.<br>\
                In each of the rest trials, you have <b> up to 1 minute</b>.\
                After 1 minute, the scatterplot will be removed.\
                There is no time limit on providing your estimation.\
        </p>\
        <p>\
            <br>Click the next button to launch VR. Ask the experimenter to help you wear the headset.\
        </p>\
      </div>",
      ":/resources/yes-yes.png", ""
    }
};
