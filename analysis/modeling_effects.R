# 
# freq_lm <- lm(AbsError ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset , data = trials)
# ols_vif_tol(freq_lm) # 1 < VIF < 1.5

N_ITER = 2100
N_WARMUP = 1100
N_THIN = 1
N_CHAINS = 4
REBUILD = F

if(REBUILD){ 

  prior <- c(set_prior("normal(0, 5)", class = "b"),
             set_prior("normal(0, 5)", class = "b", dpar = "sigma"),
             set_prior("normal(0, 50)", class = "Intercept"),
             set_prior("student_t(3, 0, 5)", class = "sd")
             )

  factor_abserror_fit <- brm(bf(AbsError ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset + (1|ParticipantID), 
                            sigma ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset), 
                            data = trials, family = "lognormal", prior = prior,
                            chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
  
  factor_reactime_fit <- brm(bf(ReactTime ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset + (1|ParticipantID), 
                            sigma ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset), 
                            data = trials, family = "lognormal", prior = prior,
                            chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
}

get_coef_plot <- function(factor_fit, ytitle, mu_scale, sd_scale, fcolor){
  coefs <- as.data.frame(fixef(factor_fit, probs = c(0.025, 0.25, 0.5, 0.75, 0.975)))
  all_cf_names <- row.names(coefs)
  
  b_sigma_NA <- sapply(all_cf_names, FUN = function(x){if((x %like% "sigma") && !(x %like% "Intercept")) return(x) else return(NA) })
  b_sigma <- row.names(data.frame(b_sigma_NA)%>% drop_na())
  b_sigma <- paste("b", b_sigma, sep="_")
  
  b_ind_NA <-  sapply(all_cf_names, FUN = function(x){if(!(x %like% "sigma") && !(x %like% "Intercept")) return(x) else return(NA) })
  b_ind <- row.names(data.frame(b_ind_NA)%>% drop_na())
  b_ind <- paste("b", b_ind, sep="_")
  
  b_var_samples <- posterior_samples(factor_fit, pars = b_ind)
  stack_b_var_samples <- stack(b_var_samples)
  stack_b_var_samples$ind = with(stack_b_var_samples, factor(ind, levels = rev(levels(ind))))
  
  b_sd_samples <- posterior_samples(factor_fit, pars = b_sigma)
  stack_b_sd_samples <- stack(b_sd_samples)
  
  coefs$b_ind <-  b_ind_NA
  coefs$b_sigma <-  b_sigma_NA
  
  coefs_ind <- coefs %>% drop_na(b_ind)
  coefs_ind <- coefs_ind %>% mutate(b_ind = paste("b", b_ind, sep = "_"))
  
  coefs_sigma <- coefs %>% drop_na(b_sigma)
  coefs_sigma <- coefs_sigma %>% mutate(b_sigma = paste("b", b_sigma, sep = "_"))
 
  g_var_mu <- g %>%
    + geom_vline(xintercept = 0, size = LINE_SIZE_NARROW, linetype  = "dashed", color = vcolor)%>%
    + geom_density_ridges(data = stack_b_var_samples, aes(x = values, y = ind), color = NA, fill = fcolor, size = 0,  rel_min_height = REF_HEIGHT, scale = 0.7)%>%
    + geom_density_ridges(data = stack_b_var_samples, aes(x = values, y = ind), color = NA, fill = fcolor, size = 0,  rel_min_height = REF_HEIGHT, scale = 0.7)%>%
    + geom_errorbarh(data = coefs_ind, aes(xmin = `Q2.5`, xmax = `Q97.5`, y = b_ind), height = 0, size = LINE_SIZE )%>%
    + geom_errorbarh(data = coefs_ind, aes(xmin = `Q25`, xmax = `Q75`, y = b_ind) , height = 0, size = LINE_SIZE_WIDE)%>%
    #+ geom_errorbarh(data = coefs_ind, aes(xmin = Q50 - .0005, xmax = Q50 + .0005, y = b_ind) , color = "white", height = 0, size = LINE_SIZE_WIDE)%>%
    + geom_point(data = coefs_ind, aes(x = Q50, y = b_ind), size = LINE_SIZE_WIDE / 5, color = "white")%>%
    + geom_text(data = coefs_ind, size = 2, aes(x = max(`Q97.5`), y = b_ind,  label = paste(sprintf("%0.4f", round(`Q50`, digits = 4))," [", sprintf("%0.4f", round(`Q2.5`, digits = 4)), "," , sprintf("%0.4f", round(`Q97.5`, digits = 4)), "]", sep = "")))%>%
    + scale_x_continuous(limits=mu_scale, breaks = seq(-0.5, 0.5, by = 0.1), expand = c(0,0))%>%
    + ggtitle(ytitle) + xlab("Estimated Effects") + ylab("Mu")
  
  g_var_sd <- g %>%
    + geom_vline(xintercept = 0, size = LINE_SIZE_NARROW, linetype  = "dashed", color = vcolor)%>%
    + geom_density_ridges(data = stack_b_sd_samples, aes(x = values, y = ind), color = NA, fill = fcolor, size = 0,  rel_min_height = REF_HEIGHT, scale = 0.7)%>%
    + geom_density_ridges(data = stack_b_sd_samples, aes(x = values, y = ind), color = NA, fill = fcolor, size = 0,  rel_min_height = REF_HEIGHT, scale = 0.7)%>%
    + geom_errorbarh(data = coefs_sigma, aes(xmin = `Q2.5`, xmax = `Q97.5`, y = b_sigma), height = 0, size = LINE_SIZE)%>%
    + geom_errorbarh(data = coefs_sigma, aes(xmin = `Q25`, xmax = `Q75`, y = b_sigma) , height = 0, size = LINE_SIZE_WIDE)%>%
    #+ geom_errorbarh(data = coefs_sigma, aes(xmin = `Q50`, xmax = `Q50`, y = b_ind) , color = "white", height = 0, size = LINE_SIZE_WIDE)%>%
    + geom_point(data = coefs_sigma, aes(x = Q50, y = b_sigma), size = DOT_SIZE_SMALL / 5, color = "white")%>%
    + geom_text(data = coefs_sigma, size = 2, aes(x = max(`Q97.5`), y = b_sigma,  label = paste(sprintf("%0.4f", round(`Q50`, digits = 4))," [", sprintf("%0.4f", round(`Q2.5`, digits = 4)), "," , sprintf("%0.4f", round(`Q97.5`, digits = 4)), "]", sep = "")))%>%
    + scale_x_continuous(limits=sd_scale, breaks = seq(-1, 1, by = 0.2), expand = c(0,0))%>%
    + ggtitle(ytitle) + xlab("Estimated Effects") + ylab("Sigma")
  
  return(list(g_var_mu, g_var_sd))
}

gg1 <- get_coef_plot(factor_abserror_fit, "Abs Error",c(-0.1, 0.21), c(-.1, .21), ABSERROR_COLOR)
gg2 <- get_coef_plot(factor_reactime_fit, "Reaction Time", c(-0.01, 0.375), c(-0.01, 0.375), REACT_COLOR)

ggsave(file = "pic/fig_effects_mu.pdf", ggarrange(gg1[[1]], gg2[[1]], widths=c(1,1.3)), width = 6, height = 2.5, units = "in")
ggsave(file = "pic/fig_effects_sigma.pdf", ggarrange(gg1[[2]], gg2[[2]], widths=c(1,1.3)), width = 6, height = 2.5, units = "in")