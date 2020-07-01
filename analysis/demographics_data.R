

demographics <- metas %>% select(familiarML, familiarDR, familiarVIS, familiarVR)
demographics_stack <- stack(demographics)
demographics_stack$overall <- paste(demographics_stack$values, demographics_stack$ind, sep = " ")


demographics_table <- aggregate(data = demographics_stack, values ~ overall, length)
demographics_table$item <- sapply(demographics_table$overall, function(x){return(strsplit(x, split = " ")[[1]][2])})  
demographics_table$option <- sapply(demographics_table$overall, function(x){return(strsplit(x, split = " ")[[1]][1])})  

g_demographics <- g_l %>%
  + geom_tile(data = demographics_table, aes(x = option, y = item, fill = values)) %>%
  + scale_fill_continuous(low = "#ffffff", high = ABSERROR_COLOR, limits = c(0,24), breaks = c(0, 8, 16, 24)) + xlab("scale") + ylab("")
g_demographics

ggsave(file = "pic/demographics.pdf", g_demographics, width = 4.6, height = 2, units = "in")

meta_trials <-merge(x=trials,y=metas,by="PariticipantID")

