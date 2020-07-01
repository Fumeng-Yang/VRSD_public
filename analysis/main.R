setwd("//Volumes/Data/Github/VRSD_public/analysis/")

# rm(list=ls())

library(ggplot2) #core graphics package
library(ggridges) # ggarrange

library(tidyverse) # data manipulation
library(forcats)

library(brms) # core pack
library(modelr)

library(RColorBrewer)
library(tidybayes) 
library(olsrr)
library(ggpubr)
library(data.table) #%like%

library(cowplot) # draw multiple figures
library(wesanderson) #colro

source("loadData.R")
source("style.R")

print("modeling_effects.R") # RQ1
source("modeling_effects.R")

print("modeling_div_effects.R")# RQ2
source("modeling_div_effects.R")

print("ranking_distribution.R") # RQ3
source("ranking_distribution.R")

print("modeling_perception.R") # RQ4
source("modeling_perception.R")

print("demographics_data.R")
source("demographics_data.R")
