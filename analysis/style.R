DOT_SIZE <- 2
DOT_SIZE_BIG <- 3
DOT_SIZE_SMALL <- 1.5
DOT_SIZE_TINY <- 0.5
AXIS_LINE_SIZE <- 0.1
LINE_SIZE <- 0.7
ACC_LINE_SIZE <- 0.25
LINE_SIZE_WIDE <- 1.3
LINE_SIZE_NARROW <- 0.5
FONT_SIZE <- 10
STROKE_SIZE <- 0.5
alphaDot <- 0.2
alphaDensity <- 0.3
alphaHalf <- 0.5
barColor <- "#E0E0E0"
# set colors
darkgray <- "#222222"
backColor <- "#FFFFFF"
grayColor <- "#AAAAAA"
textColor <- "#222222"
labelBack <- "#e7e7e7"
diffColor <- "#958bb2"
vcolor <- "#AAAAAA"
# FOURPALETTE <- c("#999999", )
FOURPALETTE <- c( "#5c87e0", "#5fad06", "#ffa100", "#999999") #"#67a82f", 
THREEPALETTE <- c( "#5c87e0", "#5fad06", "#ffa100") #"#67a82f", 
FIVEPALETTE <- c(diffColor, "#5c87e0", "#5fad06", "#ffa100", "#999999") #"#67a82f", 
CBPALETTE2 <- c("#799daf", "#E5625C", "#e29738",  "#777777" )
CBPALETTE3 <- c("#e89a1e", "#777777")

DENSITY_HEIGHT <- 0.8
REF_HEIGHT <- 0.02
Y_EXPAND <- c(0.05, 0.05)
X_EXPAND <- c(0.01, 0.01)
ACC_LINE_TYPE <- 'dashed'
ACC_LINE_COLOR <- "#BBBBBB"
MEAN_HEIGHT <- 0.85
BANDWIDTH_SMALL <- 0.025
BANDWIDTH_MID <- 0.05
BANDWIDTH_LARGE <- 0.2
BANDWIDTH_HUGE <- 0.4
BANDWIDTH_VERY_LARGE <- 0.5
ABSERROR_COLOR <- "#b6d6d3"
REACT_COLOR <- "#edbeca"#"#EAAA2A"



# average comfortable level: explanation x vis
g <- ggplot( environment = environment()) %>%
  +theme(axis.line = element_line(colour = "gray", size = AXIS_LINE_SIZE, linetype = "solid"), 
          axis.line.y = element_blank(),
          axis.line.x = element_line(size = 0.5, linetype = "solid"), 
          axis.text = element_text(size = FONT_SIZE, color = textColor,  family = 'Helvetica'),
          axis.text.x = element_text(size = FONT_SIZE, color = textColor, vjust=0.5, hjust=0.5, face="plain",  family = 'Helvetica'),
          axis.ticks = element_line(color = darkgray, size = 0.5),
          axis.title.x = element_text(size = FONT_SIZE, color = textColor, family = 'Helvetica'),
          axis.title.y = element_text(size = FONT_SIZE, color = textColor, family = 'Helvetica'),
          panel.background = element_rect(fill = backColor, size = 0, colour = NA),
          panel.grid.major.y = element_line(colour = barColor, size = 0.5, linetype = "solid"), 
          panel.grid.major.x = element_blank(),
          panel.grid.minor = element_blank(),
          panel.spacing = unit(1.2, "lines"),
          axis.ticks.length = unit(0.1, "lines"),
          strip.background =  element_rect(fill = NA), 
          strip.placement = "outside",
          strip.text = element_text(size = FONT_SIZE, face = "bold", colour = textColor, family = 'Helvetica'),
          plot.title = element_text(size = 5, hjust = 0.5, face = "bold", colour = textColor,  family = 'Helvetica'),
          legend.position = "none",
          legend.background = element_rect()) 


g_l <- ggplot( environment = environment()) %>%
  +theme(axis.line = element_line(colour = "gray", size = AXIS_LINE_SIZE, linetype = "solid"), 
         axis.line.y = element_blank(),
         axis.text = element_text(size = FONT_SIZE, color = textColor,  family = 'Helvetica'),
         axis.text.x = element_text(size = FONT_SIZE, color = textColor, vjust=0.5, hjust=0.5, face="plain",  family = 'Helvetica'),
         axis.ticks = element_line(color = darkgray),
         axis.title.x = element_text(size = FONT_SIZE, color = textColor, family = 'Helvetica'),
         axis.title.y = element_text(size = FONT_SIZE, color = textColor, family = 'Helvetica'),
         panel.background = element_rect(fill = backColor, size = 0, colour = NA),
         panel.grid.major = element_line(colour = barColor, size = AXIS_LINE_SIZE / 2, linetype = "solid"), 
         panel.grid.minor = element_blank(),
         panel.spacing = unit(1.2, "lines"),
         axis.ticks.length = unit(0.1, "lines"),
         strip.background =  element_rect(fill = NA), 
         strip.placement = "outside",
         strip.text = element_text(size = FONT_SIZE, face = "bold", colour = textColor, family = 'Helvetica'),
         plot.title = element_text(size = 5, hjust = 0.5, face = "bold", colour = textColor,  family = 'Helvetica'),
         legend.background = element_rect(size = 0, colour = NA),
         legend.text=element_text(size = FONT_SIZE, face = "bold", colour = textColor, family = 'Helvetica')) 
