blues <- c("#C6DBEF", "#3182BD", "#08519C")

N_ITER = 2100
N_WARMUP = 1100
N_THIN = 1
N_CHAINS = 2
REBUILD = T

fs = levels(trials$AllFactors)

pred_df <- data.frame()
accuracy_models <- list()

if(REBUILD){
  
    
    prior <- c(set_prior("normal(0,2)", class = "b"),
               set_prior("normal(0,5)", class = "b", dpar = "sigma"),
               set_prior("normal(0,5)", class = "Intercept"),
               set_prior("student_t(3, 0, 10)", class = "sd"))
    
    
    accuacy_fit <- brm(bf(Error ~ AllFactors*NormalAccuracy + (Dataset|ParticipantID),
                          sigma ~ AllFactors*NormalAccuracy
                          ), data = trials, family = "normal", 
                      prior = prior, chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN,
                      control = list(adapt_delta = 0.99))
    
}    
    
    new_normAccuracy <- seq_range(35:92, n = 20) / 100 - 0.5
    
    pred <- trials %>%
      data_grid(Dataset, AllFactors, ParticipantID, NormalAccuracy = new_normAccuracy) %>%
      add_predicted_draws(accuacy_fit, re_formula = NULL, allow_new_levels = TRUE)
    
    pred <- as.data.frame(pred)%>%select(c("AllFactors", "NormalAccuracy", ".prediction"))
    

g_pred <-  g %>%
  + stat_lineribbon(data = pred, aes(y = .prediction, x = (NormalAccuracy + 0.5) * 100), .width = c(.95, 0.8, .5), size = LINE_SIZE, alpha = 0.5)%>%
  + geom_point(data = trials, aes(x = (NormalAccuracy + 0.5) * 100, y = Error), alpha = 0.1, size = DOT_SIZE+0.2, stroke = 0)%>%
  + geom_hline(yintercept = 0, size = LINE_SIZE_NARROW, linetype  = "dashed")%>%
  + scale_fill_brewer(palette="Blues")%>%
  + facet_wrap(.~AllFactors, ncol = 13)%>%
  + scale_y_continuous(lim=c(-35, 35))%>%
  + scale_x_continuous(breaks=seq(40, 80, by = 20))%>%
  + ylab("Posterior Prediction of Estimation Error (%)") + xlab("True Accuracy (%)")

ggsave(file = "pic/pred_error_accuracy_student.pdf", plot = g_pred, width = 20, height = 5, unit = "in")
