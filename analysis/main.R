

rm(list=ls())

library(ggplot2) #core graphics package
library(ggridges) # ggarrange
library(rstan) # core pack
library(plyr) # data manipulation
library(dplyr) # data manipulation
library(tidyr) # data manipulation
library(forcats)
library(rstanarm) # core pack
library(brms) # core pack
library(modelr)
library(RColorBrewer)
library(tidybayes) #thanks matt kay
library(olsrr)
library(ggpubr)
library(data.table) #%like%
library(cowplot) # draw multiple figures
library(wesanderson) #colro

source("load_data.R")
source("style.R")
source("modeling_effects.R")
source("modeling_div_effects.R")
source("ranking_distribution.R")

