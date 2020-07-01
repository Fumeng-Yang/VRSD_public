# 
# freq_lm <- lm(AbsError ~ Stereo + Perspective + Motion + Shading + Dimen + Dataset , data = trials)
# ols_vif_tol(freq_lm) # 1 < VIF < 1.5

N_ITER = 2100
N_WARMUP = 1100
N_THIN = 1
N_CHAINS = 4

REBUILD = T

draw_one_effect <- function(post_ci, post, metric, fv, fcolor){
  
  post_ci$fv <- post_ci[[fv]]
  post$fv <- post[[fv]]
  
  post_ci <- post_ci %>%
    ungroup()%>%
    mutate(linewidth = (1 - sqrt(.width)) * (1 - sqrt(.width)))
  
  bw <- 1
  breakstick <- seq(0, 30, by = 10)
  lims <- c(0, 30)
  
  if(metric == "Error Magnitude"){
     bw <- 0.1
     breakstick <- seq(4, 10, by = 2)
     lims <- c(4, 10)
  }
  
  
    post_ci1 <-  post_ci %>% filter(.width > 0.94)
    post_ci2 <-  post_ci %>% filter(.width > 0.4 & .width < 0.94)
  
    g_mean <- g %>%
    + geom_density_ridges(data = post, aes(x = .mean, y = fv), rel_min_height = REF_HEIGHT, size = 0.0, color = NA, fill = fcolor, scale = 0.75, bandwidth = bw)%>%
    + geom_errorbarh(data = post_ci2, aes(xmin = .mean.lower, xmax = .mean.upper, y = fv), size = LINE_SIZE_WIDE, height = 0.00)%>%
    + geom_errorbarh(data = post_ci1, aes(xmin = .mean.lower, xmax = .mean.upper, y = fv), size = LINE_SIZE, height = 0.00)%>%
    # + geom_point(data = post_ci, aes(x = .mean, y = fv), size = LINE_SIZE_WIDE/ 4, color = "white")%>%
    # + geom_point(data = coefs_ind, aes(x = Q50, y = b_ind), size = LINE_SIZE_WIDE + LINE_SIZE_NARROW, color = "black")%>%
    + geom_point(data = post_ci, aes(x = .mean, y = fv), size = LINE_SIZE_WIDE + LINE_SIZE_NARROW, color = "black" )%>% 
    # + geom_errorbarh(data = post_ci1, aes(xmax = Q50, xmin = Q50, y = b_sigma), size = 0, height = 0.1, color = "white")%>% # , position = position_nudge(y = 0.35)
    + geom_text(data = post_ci1, size = 2, aes(x = max(.mean.upper * 1.05), y = fv,  label = paste(sprintf("%0.2f", round(.mean, digits = 4))," [", sprintf("%0.2f", round(.mean.lower, digits = 4)), ", " , sprintf("%0.2f", round(.mean.upper, digits = 4)), "]", sep = "")))%>%
    + scale_x_continuous(lim = lims, breaks = breakstick, expand = c(0.05,0.05))%>%
    + scale_y_discrete(expand = c(0.25,0))%>%
    + scale_fill_brewer(palette="Blues") + xlab("") + ylab("")
  
   g_sd <-g %>%
    + geom_density_ridges(data = post, aes(x = .sd, y = fv), size = 0.0, color = NA, fill = fcolor, scale = 0.75)%>%
    + geom_errorbarh(data = post_ci2, aes(xmin = .sd.lower, xmax = .sd.upper, y = fv), size = LINE_SIZE_WIDE, height = 0.00)%>%
    + geom_errorbarh(data = post_ci1, aes(xmin = .sd.lower, xmax = .sd.upper, y = fv), size = LINE_SIZE, height = 0.00)%>%
    # + geom_point(data = post_ci, aes(x = .sd, y = fv), size = LINE_SIZE_WIDE / 5, color = "white")%>%
    # + geom_point(data = post_ci, aes(x = .sd, y = fv), shape = 108, size = LINE_SIZE_WIDE, color = "white" )%>% 
    # + geom_errorbarh(data = post_ci, aes(xmin = .sd, xmax = .sd, y = fv), size = 0, height = 0.1, color = "white" )%>% 
    + geom_point(data = post_ci, aes(x = .sd, y = fv), size = LINE_SIZE_WIDE + LINE_SIZE_NARROW, color = "black" )%>% 
    + geom_text(data = post_ci1, size = 2, aes(x = max(.sd.upper)* 1.05, y = fv,  label = paste(sprintf("%0.2f", round(.sd, digits = 4))," [", sprintf("%0.2f", round(.sd.lower, digits = 4)), ", " , sprintf("%0.2f", round(.sd.upper, digits = 4)), "]", sep = "")))%>%
    + scale_fill_brewer(palette="Blues")%>%
    + scale_y_discrete(expand = c(0.25,0))%>%
    + xlab("") + ylab("")
  

  return(list(g_mean, g_sd))
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
    select(vStereo, .mean, .sd) %>%
    group_by(vStereo) %>% 
    median_qi(.width = c(.95, .5, .0)) %>%
    select_if(~sum(!is.na(.)) > 0) %>%
    mutate(category = "vStereo")
  
  post_2 <- post %>%  
    ungroup() %>% 
    select(vMotion, .mean, .sd) %>%
    group_by(vMotion) %>% 
    median_qi(.width = c(.95, .5, .0)) %>%
    select_if(~sum(!is.na(.)) > 0) %>%
    mutate(category = "vMotion")
  
  post_3 <- post_3d %>%  
    ungroup() %>% 
    select(vPerspective, .mean, .sd) %>%
    group_by(vPerspective) %>% 
    median_qi(.width = c(.95, .5, .0)) %>%
    select_if(~sum(!is.na(.)) > 0) %>%
    mutate(category = "vPerspective")
  
  post_4 <- post_3d %>%  
    ungroup() %>% 
    select(vShading, .mean, .sd) %>%
    group_by(vShading) %>% 
    median_qi(.width = c(.95, .5, .0)) %>%
    select_if(~sum(!is.na(.)) > 0) %>%
    mutate(category = "vShading")
  
  post_5 <- post %>%  
    ungroup() %>% 
    select(vDim, .mean, .sd) %>%
    group_by(vDim) %>% 
    median_qi(.width = c(.95, .5, .0)) %>%
    select_if(~sum(!is.na(.)) > 0)  %>%
    mutate(category = "vDim")
  
  post_6 <- post %>%  
    ungroup() %>% 
    select(vDataset, .mean, .sd) %>%
    group_by(vDataset) %>% 
    median_qi(.width = c(.95, .5, .0)) %>%
    select_if(~sum(!is.na(.)) > 0) %>%
    mutate(category = "vDataset")
  

  # print(colnames(post_6))
  uniform_colnames <- c("factor", ".mean", ".mean.lower", ".mean.upper", ".sd", ".sd.lower", ".sd.upper", ".width", ".point", ".interval", "category" )

  tmp_2 <- data.frame(post_2)
  colnames(tmp_2) <- uniform_colnames
  
  tmp_3 <- data.frame(post_3)
  colnames(tmp_3) <- uniform_colnames
  
  tmp_4 <- data.frame(post_4)
  colnames(tmp_4) <- uniform_colnames
  
  tmp_5 <- data.frame(post_5)
  colnames(tmp_5) <- uniform_colnames
  
  tmp_6 <- data.frame(post_6)
  colnames(tmp_6) <- uniform_colnames
  
  if(metric == "Error Magnitude"){
    post_abserr_map <<- data.frame(post_1)
    colnames(post_abserr_map) <- uniform_colnames
    
    post_abserr_map <<- rbind(post_abserr_map, tmp_2, tmp_3, tmp_4, tmp_5, tmp_6)
    
  }else{
    
    post_react_map <<- data.frame(post_1)
    colnames(post_react_map) <- uniform_colnames
    
    post_react_map <<- rbind(post_react_map, tmp_2, tmp_3, tmp_4, tmp_5, tmp_6)
    
  }
  
  

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
  select_if(~sum(!is.na(.)) > 0) %>%
  mutate(.mean = .value - delta) %>%
  mutate(.sd = sqrt(1 + sigma / (mu * mu) ))

post_abserr_3d <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDataset) %>%
  add_fitted_draws(vfactor_abserror_fit_3d, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0) %>%
  mutate(.mean = .value - delta)%>%
  mutate(.sd = sqrt(1 + sigma / (mu * mu) ))

post_react <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDim, vDataset) %>%
  add_fitted_draws(vfactor_reactime_fit, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0) %>%
  mutate(.mean = .value)%>%
  mutate(.sd = sqrt(1 + sigma / (mu * mu) ))

post_react_3d <- trials %>%
  data_grid(vStereo, vPerspective, vMotion, vShading, vDataset) %>%
  add_fitted_draws(vfactor_reactime_fit_3d, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma"))%>%
  select_if(~sum(!is.na(.)) > 0) %>%
  mutate(.mean = .value)%>%
  mutate(.sd = sqrt(1 + sigma / (mu * mu) ))



figs_6_abserr <- draw_div_effects(post_abserr, post_abserr_3d, "Error Magnitude", ABSERROR_COLOR)
figs_6_react <- draw_div_effects(post_react, post_react_3d, "Response Time", REACT_COLOR)
factorlist <- c("Stereo", "Motion", "Projection", "Shading", "Dim", "Dataset")



path1 <- "pic/g_post_div_var_mean.pdf"
path2 <- "pic/g_post_div_var_sd.pdf"

w <- 5
h <- 6.5

ggsave(file = path1,  plot_grid(figs_6_abserr[[1]][[1]], figs_6_react[[1]][[1]],
                                figs_6_abserr[[2]][[1]], figs_6_react[[2]][[1]],
                                figs_6_abserr[[3]][[1]], figs_6_react[[3]][[1]],
                                figs_6_abserr[[4]][[1]], figs_6_react[[4]][[1]],
                                figs_6_abserr[[5]][[1]], figs_6_react[[5]][[1]],
                                figs_6_abserr[[6]][[1]], figs_6_react[[6]][[1]],
                                ncol = 2,align="v"), width = w, height = h, units = "in")
ggsave(file = path2,  plot_grid(figs_6_abserr[[1]][[2]], figs_6_react[[1]][[2]],
                                figs_6_abserr[[2]][[2]], figs_6_react[[2]][[2]],
                                figs_6_abserr[[3]][[2]], figs_6_react[[3]][[2]],
                                figs_6_abserr[[4]][[2]], figs_6_react[[4]][[2]],
                                figs_6_abserr[[5]][[2]], figs_6_react[[5]][[2]],
                                figs_6_abserr[[6]][[2]], figs_6_react[[6]][[2]], 
                                ncol = 2,align="v"), width = w, height = h, units = "in")



#post_react_map <- subset(post_react_map, .width == 0)

#g_post_react_map <- ggplot(post_react_map, aes(y = factor)) %>%
#  + geom_tile(aes(fill = exp(mu) - delta))
