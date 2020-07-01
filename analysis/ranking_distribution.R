N_ITER = 2100
N_WARMUP = 1100
N_THIN = 1
N_CHAINS = 4

REBUILD = T

bw_mean <- 05
bw_sd <- 1

w <- 14.5
h <- 8

mean_scale_ae <- c(4, 13)
break_mean1 <- seq(4, 13, by = 3)
sd_scale_ae <-c(1, 1.25)

mean_scale_rt <- c(3, 27)
break_mean2 <- seq(3, 27, by = 6)
sd_scale_rt <- c(1, 1.25)


getDistribution <- function(post_fit, metric, mean_scale, sd_scale, break_mean, bw, flag, fcolor){
  .delta <- 0
  
  if(metric == "Error Magnitude"){
    .delta <- delta
  }
  
    post <- trials %>%
        data_grid(AllFactors) %>%
        add_fitted_draws(post_fit, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma")) %>%
        mutate(.mean = .value - .delta) %>%
        mutate(.sd = sqrt(1 + sigma / (mu * mu) ))
    
    post_ci<- post %>%
        median_qi(.width = c(.95, .5, .0))
    
    
    post_ci1 <-  post_ci %>% filter(.width > 0.94)
    post_ci2 <-  post_ci %>% filter(.width > 0.4 & .width < 0.94)
    
    
    post_ci1 <- post_ci1 %>%
      ungroup()%>%
      mutate(AllFactors = fct_reorder(AllFactors, .mean, .desc = TRUE)) 
  
    
    if(flag == 1){
      mu_factor_order <<- levels(post_ci1$AllFactors)
    }
     

    post <- post %>%
      ungroup()%>%
      mutate(AllFactors = fct_relevel(AllFactors, mu_factor_order)) 
    
        d <- subset(post_ci, .width = 0)
    
    min_mean <- min(d$.mean)
    max_mean <- max(d$.mean)
  
    g_mean <-g %>%
        + geom_vline(xintercept = min_mean, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
        + geom_vline(xintercept = max_mean, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
        + geom_density_ridges(data = post, aes(x = .mean, y = AllFactors), fill = fcolor,size = 0.0, color = NA, scale = 0.85, rel_min_height = REF_HEIGHT)%>%
        + geom_errorbarh(data = post_ci2, aes(xmin = .mean.lower, xmax = .mean.upper, y = AllFactors), size = LINE_SIZE_WIDE, height = 0.00)%>%
        + geom_errorbarh(data = post_ci1, aes(xmin = .mean.lower, xmax = .mean.upper, y = AllFactors), size = LINE_SIZE, height = 0.00)%>%
        # + geom_errorbarh(data = post_ci, aes(xmax = .mean, xmin = .mean, y = AllFactors), size = 0, height = 0.1, color = "white")%>%
        + geom_point(data = post_ci, aes(x = .mean, y = AllFactors), size = LINE_SIZE_WIDE + LINE_SIZE_NARROW, color = "black")%>%
        + geom_text(data = post_ci1, size = 2, aes(x = max(.mean.upper) * 1.1, y = AllFactors,  label = paste(sprintf("%0.2f", round(.mean, digits = 4))," [", sprintf("%0.2f", round(.mean.lower, digits = 4)), ", " , sprintf("%0.2f", round(.mean.upper, digits = 4)), "]", sep = "")))%>%
        + scale_fill_brewer(palette="Blues")%>%
        + scale_x_continuous(expand = c(0.05,0.05), limits = mean_scale, breaks = break_mean)%>%
        + scale_y_discrete(expand = c(0.01,0))%>%
        + xlab(paste("Posterior Mean ", metric)) + ylab("AllFactors")+ ggtitle("mean")
   
  
    min_sd <- min(d$.sd)
    max_sd <- max(d$.sd)
  
    
    g_sd <-g %>%
    + geom_vline(xintercept = min_sd, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
    + geom_vline(xintercept = max_sd, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
    + geom_density_ridges(data = post, aes(x = .sd, y = AllFactors), fill = fcolor, color = NA, size = 0.0, scale = 0.85, rel_min_height = REF_HEIGHT)%>%
    + geom_errorbarh(data = post_ci2, aes(xmin = .sd.lower, xmax = .sd.upper, x = .sd, y = AllFactors), size = LINE_SIZE_WIDE, height = 0.00)%>%
    + geom_errorbarh(data = post_ci1, aes(xmin = .sd.lower, xmax = .sd.upper, x = .sd, y = AllFactors), size = LINE_SIZE,  height = 0.00)%>%
    #+ geom_point(data = post_ci, aes(x = .sd, y = AllFactors), size =  LINE_SIZE_WIDE / 5, color = "white" )%>%
    + geom_point(data = post_ci, aes(x = .sd, y = AllFactors), shape = 108, size = LINE_SIZE_WIDE / 2, color = "white" )%>% 
    + geom_text(data = post_ci1, size = 2, aes(x = max(.sd.upper)* 1.05, y = AllFactors,  label = paste(sprintf("%0.2f", round(.sd, digits = 4))," [", sprintf("%0.2f", round(.sd.lower, digits = 4)), ", " , sprintf("%0.2f", round(.sd.upper, digits = 4)), "]", sep = "")))%>%
    + scale_fill_brewer(palette="Blues")%>%
    + scale_x_continuous(expand = c(0.05,0.05), limits = sd_scale)%>%
    + scale_y_discrete(expand = c(0.01,0))%>%
    + xlab(paste("Posterior Mean", metric)) + ylab("AllFactors")+ ggtitle("sd")
    
    target_color <- REACT_COLOR
    breaks_seq <- seq(0,20, by = 3)
    
    if(metric == "Error Magnitude"){
      target_color = ABSERROR_COLOR
      breaks_seq <- seq(5,10, by = 0.5)
    }
    
    g_mean_tile <- g_l %>%
      + geom_tile(data = post_ci, aes(x = 1, y = AllFactors, fill = .mean))%>% 
      + scale_fill_continuous(low = "#ffffff", high = target_color, breaks = breaks_seq) %>%
      + xlab(paste("Posterior Mean Tile ", metric)) + ylab("AllFactors")+ ggtitle("mean")
    
  
    
    g_sd_tile <- g_l %>%
      + geom_tile(data = post_ci, aes(x = 1, y = AllFactors, fill = .sd))%>% 
      + scale_fill_continuous(low = "#ffffff", high = target_color, breaks_seq) %>%
      + xlab(paste("Posterior sd Tile ", metric)) + ylab("AllFactors")+ ggtitle("sd")
    
    return(list(g_mean, g_sd, g_mean_tile, g_sd_tile))
}

if(REBUILD){
    prior_abs <- c(set_prior("normal(0, 5)", class = "b"),
                   set_prior("normal(0, 5)", class = "b", dpar = "sigma"),
                   set_prior("normal(0, 50)", class = "Intercept"),
                   set_prior("student_t(3, 0, 5)", class = "sd"))
    
    
    post_fit_abserror <- brm(bf(AbsError ~ AllFactors + (Dataset|ParticipantID), sigma ~ AllFactors) , data = trials, family = "lognormal", 
          prior = prior_abs,
          chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)
    
    post_fit_reacttime <- brm(bf(ReactTime ~ AllFactors + (Dataset|ParticipantID), sigma ~ AllFactors) , data = trials, family = "lognormal",
          prior = prior_abs,
          chains = N_CHAINS, iter = N_ITER, warmup = N_WARMUP, thin = N_THIN)

}

gs_abs <- getDistribution(post_fit_abserror, "Error Magnitude", mean_scale_ae, sd_scale_ae, break_mean1, bw_mean, 1, ABSERROR_COLOR)
gs_reacttime <- getDistribution(post_fit_reacttime, "Response Time", mean_scale_rt, sd_scale_rt, break_mean2, bw_sd, 2, REACT_COLOR)

path1 <- "pic/fig_post_mean_rank.pdf"
path2 <- "pic/fig_post_sd_rank.pdf"

path3 <- "pic/fig_post_mean_tiles.pdf"
path4 <- "pic/fig_post_sd_tiles.pdf"

ggsave(file = path1,ggarrange(gs_abs[[1]], gs_reacttime[[1]], widths=c(1,1.05)), width = w, height = h, units = "in")
ggsave(file = path2, ggarrange(gs_abs[[2]], gs_reacttime[[2]], widths=c(1,1.05)), width = w, height = h, units = "in")


ggsave(file = path3,ggarrange(gs_abs[[3]], gs_reacttime[[3]], widths=c(1,1.05)), width = w, height = h, units = "in")
ggsave(file = path4, ggarrange(gs_abs[[4]], gs_reacttime[[4]], widths=c(1,1.05)), width = w, height = h, units = "in")