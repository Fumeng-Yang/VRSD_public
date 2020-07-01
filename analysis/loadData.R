
read.csv.row_null <- function(p){
  return(read.csv(p, stringsAsFactors = F, header = T, sep=","))
}

delta <- 1

metas <- read_csv(path = "../data/metas.csv", header = T)
trials <- read_csv(path = "../data/trials.csv", header = T)

trials<-subset(trials, TrialStatus == "main") #drop practice trials

trials$ParticipantID <- trials$PariticipantID # fix typo

trials$Error <- trials$TrialAnswer - trials$TrialIntAccuracy
trials$AbsError <- abs(trials$Error) + delta
trials$LogAbsError <- log(abs(trials$Error) + 1)
trials$NormalAccuracy <- trials$TrialIntAccuracy / 100 - 0.5
trials$ReactTime <- (trials$TrialVisualizationEnd - trials$TrialVisualizationStart)/ 1000
trials$LogReactTime <- log(trials$ReactTime)
trials$ParticipantID <- factor(trials$ParticipantID)
trials$Accuracy <- trials$TrialIntAccuracy

trials <- subset(trials,!(ParticipantID == "P7" & SessionIndex == 0 & TrialStatus == "main" & TrialIndex == 16)) #double check
trials <- subset(trials,!(ParticipantID == "P9" & SessionIndex == 2 & TrialStatus == "main" & TrialIndex == 17))
trials <- subset(trials,!(ParticipantID == "P11" & SessionIndex == 0 & TrialStatus == "main"  & TrialIndex == 12)) 
trials <- subset(trials,!(ParticipantID == "P20" & SessionIndex == 3 & TrialStatus == "main" & TrialIndex == 5)) 
trials <- subset(trials,!(ParticipantID == "P24" & SessionIndex == 0 & TrialStatus == "main"  & TrialIndex == 26))

trials <- subset(trials, AbsError < 51)

trials <- trials %>%
  mutate(Stereo = recode(FactorStereo, yes = 1, no =-1))%>%
  mutate(Motion = recode(FactorMotion, yes = 1, no =-1))%>%
  mutate(Perspective = recode(FactorPerspective, yes = 1, no =-1))%>%
  mutate(Shading = recode(FactorShading, `shading-1` = 0, `shading-2` =1, `flat`=-1, `no` = -1))%>%
  mutate(vStereo = recode(FactorStereo, yes = "HMD", no ="Desktop"))%>%
  mutate(vMotion = recode(FactorMotion, yes = "Motion", no ="No Motion"))%>%
  mutate(vPerspective = recode(FactorPerspective, yes = "Perspective", no ="Orthographic"))%>%
  mutate(vShading = recode(FactorShading, `flat`="Flat Shading", `no` = "Flat Shading", `shading-1` = "Simple Shading", `shading-2` = "AO Shading"))%>%
  mutate(vDim = recode(Dim, `2` = "2D t-SNE", `3` = "3D t-SNE"))%>%
  mutate(Dimen = recode(Dim, `2` = -1, `3` = 1))%>%
  mutate(vShadingDimen = paste(vShading, vDim, sep='?'))%>%
  mutate(ShadingDimen = recode(vShadingDimen, "AO Shading?3D t-SNE" = 1, "Flat Shading?2D t-SNE" = -1,   "Flat Shading?3D t-SNE" = -0.5,  "Simple Shading?3D t-SNE" = 0))%>%
  mutate(vDataset = recode(TrialDataset, "cifar10" = "CIFAR10", "babi-q1" = "bAbI"))%>%
  mutate(AllFactors = paste(vStereo, vMotion, vPerspective, vShading, vDim, sep=" ? "))%>%
  mutate(Dataset = recode(TrialDataset, "cifar10" = 1, "babi-q1" = -1))

trials$ParticipantID <- factor(trials$ParticipantID)
trials$Accuracy <- trials$TrialIntAccuracy

trials$AllFactors <- as.factor(trials$AllFactors)
#trials$Factors <- NA

# error trials double check with data
trials_3d <- subset(trials, Dim == "3")