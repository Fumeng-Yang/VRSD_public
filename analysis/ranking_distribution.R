blues <- c("#C6DBEF", "#3182BD", "#08519C")

N_ITER = 2100
N_WARMUP = 1100
N_THIN = 1
N_CHAINS = 4

REBUILD = F

bw_mu <- 0.1 
bw_sigma <- 1

w <- 14
h <- 8

mu_scale_ae <- c(2.5, 8.5)
break_mu1 <- seq(2.5, 8.5, by = 2)
sigma_scale_ae <-c(.7, 2.0)

mu_scale_rt <- c(2, 23)
break_mu2 <- seq(2, 23, by = 7)
sigma_scale_rt <- c(0, 2)


getDistribution <- function(post_fit, metric, mu_scale, sigma_scale, break_mu, bw, flag, fcolor){
    post <- trials %>%
        data_grid(AllFactors) %>%
        add_fitted_draws(post_fit, re_formula = NA, allow_new_levels = TRUE, dpar = c("mu", "sigma")) 
    
    post_ci<- post %>%
        median_qi(.width = c(.95, .5, .0))

    post <- post%>% ungroup()%>% mutate(mu = exp(mu) - delta)
    post_ci <- post_ci%>% ungroup()%>%mutate(mu = exp(mu) - delta) %>% mutate(mu.lower = exp(mu.lower) - delta) %>% mutate(mu.upper = exp(mu.upper) - delta)
    
    
    post_ci1 <-  post_ci %>% filter(.width > 0.94)
    post_ci2 <-  post_ci %>% filter(.width > 0.4 & .width < 0.94)
    
    
    post_ci1 <- post_ci1 %>%
      ungroup()%>%
      mutate(AllFactors = fct_reorder(AllFactors, mu.lower, .desc = TRUE)) 
  
    
    if(flag == 1){
      mu_factor_order <<- levels(post_ci1$AllFactors)
    }
     

    post <- post %>%
      ungroup()%>%
      mutate(AllFactors = fct_relevel(AllFactors, mu_factor_order)) 
    
        d <- subset(post_ci, .width = 0)
    min_mu <- min(d$mu)
    max_mu <- max(d$mu)
  
    g_mu <-g %>%
        + geom_vline(xintercept = min_mu, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
        + geom_vline(xintercept = max_mu, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
        + geom_density_ridges(data = post, aes(x = mu, y = AllFactors), fill = fcolor,size = 0.0, color = NA, scale = 0.85, rel_min_height = REF_HEIGHT)%>%
        + geom_errorbarh(data = post_ci2, aes(xmin = mu.lower, xmax = mu.upper, y = AllFactors), size = LINE_SIZE_WIDE, height = 0.00)%>%
        + geom_errorbarh(data = post_ci1, aes(xmin = mu.lower, xmax = mu.upper, y = AllFactors), size = LINE_SIZE, height = 0.00)%>%
        + geom_point(data = post_ci, aes(x = mu, y = AllFactors), size = LINE_SIZE_WIDE / 5, color = "white" )%>% 
        + geom_text(data = post_ci1, size = 2, aes(x = max(mu.upper), y = AllFactors,  label = paste(sprintf("%0.2f", round(mu, digits = 4))," [", sprintf("%0.2f", round(mu.lower, digits = 4)), ", " , sprintf("%0.2f", round(mu.upper, digits = 4)), "]", sep = "")))%>%
        + scale_fill_brewer(palette="Blues")%>%
        + scale_x_continuous(expand = c(0,0), limits = mu_scale, breaks = break_mu)%>%
        + xlab(paste("Posterior Mean ", metric)) + ylab("AllFactors")+ ggtitle("Mu")
   
  
      
    
    min_sigma <- min(d$sigma)
    max_sigma <- max(d$sigma)
  
    
    g_sd <-g %>%
    + geom_vline(xintercept = min_sigma, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
    + geom_vline(xintercept = max_sigma, size = LINE_SIZE_NARROW, color = vcolor, linetype= "dashed")%>%
    + geom_density_ridges(data = post, aes(x = sigma, y = AllFactors), fill = fcolor, color = NA, size = 0.0, scale = 0.85, rel_min_height = REF_HEIGHT)%>%
    + geom_errorbarh(data = post_ci2, aes(xmin = sigma.lower, xmax = sigma.upper, x = sigma, y = AllFactors), size = LINE_SIZE_WIDE, height = 0.00)%>%
    + geom_errorbarh(data = post_ci1, aes(xmin = sigma.lower, xmax = sigma.upper, x = sigma, y = AllFactors), size = LINE_SIZE,  height = 0.00)%>%
    + geom_point(data = post_ci, aes(x = sigma, y = AllFactors), size =  LINE_SIZE_WIDE / 5, color = "white" )%>% 
    + geom_text(data = post_ci1, size = 2, aes(x = max(sigma.upper), y = AllFactors,  label = paste(sprintf("%0.2f", round(sigma, digits = 4))," [", sprintf("%0.2f", round(sigma.lower, digits = 4)), ", " , sprintf("%0.2f", round(sigma.upper, digits = 4)), "]", sep = "")))%>%
    + scale_fill_brewer(palette="Blues")%>%
    + scale_x_continuous(expand = c(0,0), limits = c(0.2,1.5))%>%
    + xlab(paste("Posterior Mean", metric)) + ylab("AllFactors")+ ggtitle("sigma")
    
    return(list(g_mu, g_sd))
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

gs_abs <- getDistribution(post_fit_abserror, "Absolute Estimation Error", mu_scale_ae, sigma_scale_ae, break_mu1, bw_mu, 1, ABSERROR_COLOR)
gs_reacttime <- getDistribution(post_fit_reacttime, "Reaction Time", mu_scale_rt, sigma_scale_rt, break_mu2, bw_sigma, 2, REACT_COLOR)

path1 <- "pic/fig_post_mu_rank.pdf"
path2 <- "pic/fig_post_sigma_rank.pdf"

ggsave(file = path1,ggarrange(gs_abs[[1]], gs_reacttime[[1]], widths=c(1,1.05)), width = w, height = h, units = "in")
ggsave(file = path2, ggarrange(gs_abs[[2]], gs_reacttime[[2]], widths=c(1,1.05)), width = w, height = h, units = "in")