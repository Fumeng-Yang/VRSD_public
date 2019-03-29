
trials_abserror_sd <- aggregate(data = trials, LogAbsError ~ AllFactors, sd)

trials_error_mean <- aggregate(data = trials, Error ~ AllFactors, median)
trials_error_mean <- trials_error_mean %>%
  mutate(AllFactors = fct_reorder(AllFactors, Error)) 

g_error_mean <- g %>%
  + geom_point(data = trials_error_mean, aes(x = Error, y = AllFactors), size = DOT_SIZE) %>%
  + geom_point(data = trials, aes(x = Error, y = AllFactors), size = DOT_SIZE, alpha = 0.1) %>%
  + geom_density_ridges(data = trials, aes(x = Error, y = AllFactors, color = Dataset), bandwidth = 3,  size = 0.0, scale = 0.8)%>%
  + geom_vline(xintercept = 0, linetype = ACC_LINE_TYPE, color = ACC_LINE_COLOR, size = ACC_LINE_SIZE) %>%
  + ggtitle("estimation error")
   #+ scale_x_continuous(lim=c(-10,50))
g_error_mean


trials_abserror_mean <- aggregate(data = trials, LogAbsError ~ AllFactors, mean)
trials_abserror_mean <- trials_abserror_mean %>%
  mutate(AllFactors = fct_reorder(AllFactors, LogAbsError))  %>%
  mutate(LogAbsError = exp(LogAbsError)) 

g_abserror_mean <- g %>%
  + geom_point(data = trials_abserror_mean, aes(x = AbsError, y = AllFactors), size = DOT_SIZE) %>%
  + geom_point(data = trials, aes(x = AbsError, y = AllFactors), size = DOT_SIZE, alpha = 0.1) %>%
  + geom_density_ridges(data = trials, aes(x = AbsError, y = AllFactors, color = Dataset),  size = 0.0, scale = 0.8)%>%
  + geom_vline(xintercept = 0, linetype = ACC_LINE_TYPE, color = ACC_LINE_COLOR, size = ACC_LINE_SIZE) %>%
  + ggtitle("estimation abs error")
g_abserror_mean


trials_react_mean <- aggregate(data = trials, ReactTime ~ AllFactors, mean)
trials_react_mean <- trials_react_mean %>%
  mutate(AllFactors = fct_reorder(AllFactors, ReactTime)) 

g_react_mean <- g %>%
  + geom_point(data = trials_react_mean, aes(x = ReactTime, y = AllFactors), size = DOT_SIZE) %>%
  + geom_point(data = trials, aes(x = ReactTime, y = AllFactors), size = DOT_SIZE, alpha = 0.1) %>%
  + geom_density_ridges(data = trials, aes(x = ReactTime, y = AllFactors, color = Dataset), size = 0.0, scale = 0.8)%>%
  + geom_vline(xintercept = 0, linetype = ACC_LINE_TYPE, color = ACC_LINE_COLOR, size = ACC_LINE_SIZE) %>%
  + ggtitle("estimation react time")
g_react_mean


pdf("pic/overview-error.pdf", width = 6, height = 7, useDingbats = F)
g_error_mean
dev.off()


pdf("pic/overview-abserror.pdf", width = 6, height = 7, useDingbats = F)
g_abserror_mean
dev.off()

pdf("pic/overview-react.pdf", width = 6, height = 7, useDingbats = F)
g_react_mean
dev.off()
