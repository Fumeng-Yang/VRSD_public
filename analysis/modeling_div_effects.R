# 
# freq_lm <- lm(AbsError ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset , data = trials)
# ols_vif_tol(freq_lm) # 1 < VIF < 1.5

N_ITER = 2100
N_WARMUP = 1100
N_THIN = 1
N_CHAINS = 4

REBUILD = F

draw_one_effect <- function(post_ci, post, metric, fv, fcolor){
  
  post_ci$fv <- post_ci[[fv]]
  post$fv <- post[[fv]]

  # post <- post%>% ungroup()%>% mutate(mu = .value)
  # post_ci <- post_ci%>% ungroup()%>%mutate(mu = .value) %>% mutate(mu.lower = .value.lower) %>% mutate(mu.upper = .value.upper)
  
  post <- post%>% ungroup()%>% mutate(mu = exp(mu) - delta)
  post_ci <- post_ci%>% ungroup()%>%mutate(mu = exp(mu) - delta) %>% mutate(mu.lower = exp(mu.lower) - delta) %>% mutate(mu.upper = exp(mu.upper) - delta)
  
  
  post_ci1 <-  post_ci %>% filter(.width > 0.94)
  post_ci2 <-  post_ci %>% filter(.width > 0.4 & .width < 0.94)
  
  
  post_ci <- post_ci %>%
    ungroup()%>%
    mutate(linewidth = (1 - sqrt(.width)) * (1 - sqrt(.width)))
  
  
  bw <- 0.4
  breakstick <- seq(1, 25, by = 12)
  lims <- c(1, 25)
  
  if(metric == "Abs Error"){
     bw <- 0.05
     breakstick <- seq(3, 7, by = 2)
     lims <- c(3, 7)
  }else
     bw <- 2
  
    g_mu <- g %>%
    + geom_density_ridges(data = post, aes(x = mu, y = fv), rel_min_height = REF_HEIGHT, size = 0.0, color = NA, fill = fcolor, scale = 0.8)%>%
    + geom_errorbarh(data = post_ci2, aes(xmin = mu.lower, xmax = mu.upper, y = fv), size = LINE_SIZE_WIDE, height = 0.00)%>%
    + geom_errorbarh(data = post_ci1, aes(xmin = mu.lower, xmax = mu.upper, y = fv), size = LINE_SIZE, height = 0.00)%>%
    + geom_point(data = post_ci, aes(x = mu, y = fv), size = LINE_SIZE_WIDE/ 4, color = "white")%>%
    + geom_text(data = post_ci1, size = 2, aes(x = max(mu.upper), y = fv,  label = paste(sprintf("%0.2f", round(mu, digits = 4))," [", sprintf("%0.2f", round(mu.lower, digits = 4)), ", " , sprintf("%0.2f", round(mu.upper, digits = 4)), "]", sep = "")))%>%
    + scale_x_continuous(lim = lims, breaks = breakstick, expand = c(0,0))%>%
    + scale_fill_brewer(palette="Blues") + xlab("") + ylab("")
  
  g_sigma <-g %>%
    + geom_density_ridges(data = post, aes(x = sigma, y = fv), size = 0.0, color = NA, fill = fcolor, scale = 0.8)%>%
    + geom_errorbarh(data = post_ci2, aes(xmin = sigma.lower, xmax = sigma.upper, y = fv), size = LINE_SIZE_WIDE, height = 0.00)%>%
    + geom_errorbarh(data = post_ci1, aes(xmin = sigma.lower, xmax = sigma.upper, y = fv), size = LINE_SIZE, height = 0.00)%>%
    + geom_point(data = post_ci, aes(x = sigma, y = fv), size = LINE_SIZE_WIDE / 5, color = "white")%>%
    + geom_text(data = post_ci1, size = 2, aes(x = max(sigma.upper), y = fv,  label = paste(sprintf("%0.2f", round(sigma, digits = 4))," [", sprintf("%0.2f", round(sigma.lower, digits = 4)), ", " , sprintf("%0.2f", round(sigma.upper, digits = 4)), "]", sep = "")))%>%
    + scale_fill_brewer(palette="Blues")%>%
    + xlab("") + ylab("")
  

  return(list(g_mu, g_sigma))
}



draw_div_effects <- function(post, post_3d, metric, fcolor){
    
  post$vStereo <- ordered(post$vStereo,
                  levels = c("HMD", "Desktop"))
  
  post$vMotion <- ordered(post$vMotion,
                         levels = c("Motion", "No Motion"))
  
  post$vPerspective <- ordered(post$vPerspective,
                         levels = c("Perspective", "Orthographic"))
  
  post$vShading <- ordered(post$vShading,
                         levels = c("AO Shading", "Simple Shading", "Flat Shading"))
  
  post$vDataset <- ordered(post$vDataset,
                         levels = c( "CIFAR10", "bAbI"))
  
  post$vDim <- ordered(post$vDim,
                         levels = c("3D t-SNE", "2D t-SNE"))
  
  post_1 <- post %>%  
    ungroup() %>% 
    select(vStereo, mu, sigma, .value) %>%
    group_by(vStereo) %>% 
    median_qi(.width = c(.95, .5, .0))
  
  post_2 <- post %>%  
    ungroup() %>% 
    select(vMotion, mu, sigma, .value) %>%
    group_by(vMotion) %>% 
    median_qi(.width = c(.95, .5, .0))
  
  post_3 <- post_3d %>%  
    ungroup() %>% 
    select(vPerspective, mu, sigma, .value) %>%
    group_by(vPerspective) %>% 
    median_qi(.width = c(.95, .5, .0))
  
  post_4 <- post_3d %>%  
    ungroup() %>% 
    select(vShading, mu, sigma, .value) %>%
    group_by(vShading) %>% 
    median_qi(.width = c(.95, .5, .0))
  
  post_5 <- post %>%  
    ungroup() %>% 
    select(vDim, mu, sigma, .value) %>%
    group_by(vDim) %>% 
    median_qi(.width = c(.95, .5, .0))
  
  post_6 <- post %>%  
    ungroup() %>% 
    select(vDataset, mu, sigma, .value) %>%
    group_by(vDataset) %>% 
    median_qi(.width = c(.95, .5, .0))
  

  figs_1 <- draw_one_effect(post_1, post, metric, "vStereo", fcolor)
  figs_2 <-draw_one_effect(post_2, post_3d, metric, "vMotion", fcolor)
  figs_3 <-draw_one_effect(post_3, post_3d, metric, "vPerspective", fcolor)
  figs_4 <-draw_one_effect(post_4, post_3d, metric, "vShading", fcolor)
  figs_5 <-draw_one_effect(post_5, post, metric, "vDim", fcolor)
  figs_6 <-draw_one_effect(post_6, post, metric, "vDataset", fcolor)
  
  return(list(figs_1, figs_2, figs_3, figs_4, figs_5, figs_6))
}


if(REBUILD){ 
  
  priors <- c(set_prior("normal(0, 5)", class = "b"),
              set_prior("normal(0, 5)", class = "b", dpar = "sigma"),
              set_prior("normal(0, 50)", class = "Intercept"),
              set_prior("student_t(3, 0, 5)", class = "sd"))


  
  vfactor_abserror_fit <- brm(bf(AbsError ~ vStereo + vPerspective +  vMotion + vShading + vDim +  vDataset + (1|ParticipantID), 
                                 sigma ~ vStereo + vPerspective + vMotion + vShading + vDim + vDataset ),
                              data = trials, family = "lognormal", prior = priors,
                              chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
  
  vfactor_abserror_fit_3d <- brm(bf(AbsError ~ vStereo + vPerspective +  vMotion + vShading +  vDataset + (1|ParticipantID),
                                    sigma ~ vStereo + vPerspective + vMotion + vShading + vDataset ),
                                 data = trials_3d, family = "lognormal", prior = priors,
                                 chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
  
  vfactor_reactime_fit <- brm(bf(ReactTime ~ vStereo +  vPerspective +  vMotion + vShading+ vDim + vDataset+ (1|ParticipantID),
                                 sigma ~ vStereo + vPerspective + vMotion + vShading+ vDim + vDataset),
                              data = trials, family = "lognormal",  prior = priors,
                              chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
  
  vfactor_reactime_fit_3d <- brm(bf(ReactTime ~ vStereo +  vPerspective + vMotion + vShading + vDataset+ (1|ParticipantID),
                                 sigma ~ vStereo + vPerspective + vMotion + vShading + vDataset),
                                 data = trials_3d, family = "lognormal",  prior = priors,
                                 chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
}

post_abserr <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDim, vDataset) %>%
  add_fitted_draws(vfactor_abserror_fit, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0)

post_abserr_3d <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDataset) %>%
  add_fitted_draws(vfactor_abserror_fit_3d, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0)

post_react <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDim, vDataset) %>%
  add_fitted_draws(vfactor_reactime_fit, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0)

post_react_3d <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDataset) %>%
  add_fitted_draws(vfactor_reactime_fit_3d, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0)



figs_6_abserr <- draw_div_effects(post_abserr, post_abserr_3d, "Abs Error", ABSERROR_COLOR)
figs_6_react <- draw_div_effects(post_react, post_react_3d, "React Time", REACT_COLOR)
factorlist <- c("Stereo", "Motion", "Projection", "Shading", "Dim", "Dataset")



path1 <- "pic/g_post_div_var_mu.pdf"
path2 <- "pic/g_post_div_var_mu2.pdf"
path3 <- "pic/g_post_div_var_sigma.pdf"

w <- 4.75
h <- 1.2

# ggsave(file = "pic/eff-stereo.pdf",  plot_grid(figs_6_abserr[[1]][[1]], figs_6_react[[1]][[1]], ncol = 2,align="v"), width = w, height = h, units = "in")
# ggsave(file = "pic/eff-motion.pdf",  plot_grid(figs_6_abserr[[2]][[1]], figs_6_react[[2]][[1]], ncol = 2,align="v"), width = w, height = h, units = "in")
# ggsave(file = "pic/eff-pp.pdf",  plot_grid(figs_6_abserr[[3]][[1]], figs_6_react[[3]][[1]], ncol = 2,align="v"), width = w, height = h, units = "in")
# ggsave(file = "pic/eff-shading.pdf",  plot_grid(figs_6_abserr[[4]][[1]], figs_6_react[[4]][[1]], ncol = 2,align="v"), width = w, height = h * 1.2, units = "in")
# ggsave(file = "pic/eff-dim.pdf",  plot_grid(figs_6_abserr[[5]][[1]], figs_6_react[[5]][[1]], ncol = 2,align="v"), width = w, height = h, units = "in")
# ggsave(file = "pic/eff-dataset.pdf",  plot_grid(figs_6_abserr[[6]][[1]], figs_6_react[[6]][[1]], ncol = 2,align="v"), width = w, height = h, units = "in")


ggsave(file = path1,  plot_grid(figs_6_abserr[[1]][[1]],figs_6_abserr[[2]][[1]],figs_6_abserr[[3]][[1]],figs_6_abserr[[4]][[1]],figs_6_abserr[[5]][[1]],figs_6_abserr[[6]][[1]],
                                figs_6_react[[1]][[1]],figs_6_react[[2]][[1]],figs_6_react[[3]][[1]],figs_6_react[[4]][[1]],figs_6_react[[5]][[1]],figs_6_react[[6]][[1]],
                                ncol = 6,align="v"), width = 15, height = 2.4, units = "in")

ggsave(file = path2,  plot_grid(figs_6_abserr[[1]][[1]], figs_6_react[[1]][[1]],
                                figs_6_abserr[[2]][[1]], figs_6_react[[2]][[1]],
                                figs_6_abserr[[3]][[1]], figs_6_react[[3]][[1]],
                                figs_6_abserr[[4]][[1]], figs_6_react[[4]][[1]],
                                figs_6_abserr[[5]][[1]], figs_6_react[[5]][[1]],
                                figs_6_abserr[[6]][[1]], figs_6_react[[6]][[1]],
                                ncol = 2,align="v"), width = 5, height = 7.25, units = "in")
ggsave(file = path3,  plot_grid(figs_6_abserr[[1]][[2]], figs_6_react[[1]][[2]],
                                figs_6_abserr[[2]][[2]], figs_6_react[[2]][[2]],
                                figs_6_abserr[[3]][[2]], figs_6_react[[3]][[2]],
                                figs_6_abserr[[4]][[2]], figs_6_react[[4]][[2]],
                                figs_6_abserr[[5]][[2]], figs_6_react[[5]][[2]],
                                figs_6_abserr[[6]][[2]], figs_6_react[[6]][[2]], 
                                ncol = 2,align="v"), width = 4.75, height = 7, units = "in")
