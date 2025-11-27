#************** test*******************#
library(ggplot2)
library(ggbreak)
# ggbreak는 물결처리 용도 (중요X)

getwd()
virtualData <- scan("data/virtualData.csv.txt")
print(virtualData)

# 5cm 거리일 때 불꽃감지센서 데이터
flame5cm <- scan("C:data/dataof5cm.txt", sep = ",", what = list(NULL, 0))
flame5cm <- (flame5cm)[[2]]

matrix(flame5cm, nrow = length(flame5cm), ncol = 1,
       dimnames = list(1:600, c("5cm")));

# 10cm 거리일 때 불꽃감지센서 데이터

flame10cm <- scan("C:data/dataof10cm.txt", sep = ",", what = list(NULL, 0))
flame10cm <- (flame10cm)[[2]]
matrix(flame10cm, nrow = length(flame5cm), ncol = 1,
       dimnames = list(NULL, c("10cm")));

# 15cm 거리일 때 불꽃감지센서 데이터

flame15cm <- scan("C:data/dataof15cm.txt", sep = ",", what = list(NULL, 0))
flame15cm
flame15cm <- (flame15cm)[[2]]
matrix(flame15cm, nrow = length(flame15cm), ncol = 1,
       dimnames = list(NULL, c("15cm")));
flame15cm[600]  <- 88

#열로 묶기

flamedata <- cbind("5cm" = flame5cm, "10cm" = flame10cm, "15cm" = flame15cm)
기

flamedata <-  as.data.frame(flamedata)
flamedata
# 표준편차 데이터
scaledflamedata <- head(scale(flamedata))
head(scaledflamedata)

# 시각화 전에 stack 처리하기

stackedflamedata <- stack(flamedata)
stackedflamedata

# 거리별 표준편차

coledflamedata <- apply(flamedata, 2, sd)
coledflamedata

head(stackedflamedata)

flame5cm <- stack(as.data.frame(flame5cm))
flame10cm <- stack(as.data.frame(flame10cm))
flame15cm <- stack(as.data.frame(flame15cm))

Eflame <- rbind(flame5cm, flame10cm, flame15cm)
# 데이터 산점도 

ggplot(stackedflamedata, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 16, size = 3) + labs(x = "거리", y = "아날로그 반환값") + scale_color_gradientn(colors = c("#00003F", "#FF0000"))

ggplot(flame5cm, mapping = aes(x = values, y = values)) + geom_point(aes(color = values), shape = 16, size = 3) + labs(x = "거리", y = "아날로그 반환값") + scale_color_gradientn(colors = c("#00003F", "#FF0000"))

ggplot(flame10cm, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 16, size = 3) + labs(x = "거리", y = "아날로그 반환값") + scale_color_gradientn(colors = c("#00003F", "#FF0000"))

ggplot(flame15cm, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 16, size = 3) + labs(x = "거리", y = "아날로그 반환값") + scale_color_gradientn(colors = c("#00003F", "#FF0000"))


ggplot(Eflame, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 16, size = 3) + labs(x = "거리", y = "아날로그 반환값") + scale_color_gradientn(colors = c("#00003F", "#FF0000"))

# 표준편차 그래프

ggplot(coledflamedata, mapping = aes(x = ind, y = values)) + geom_col(aes(fill = values), width = 0.6) + labs(x = "거리", y = "표준편차") + scale_fill_gradientn(colors = c("#00003F", "#CF2222"))

flame5cm <-  flame5cm[,2]

flame5cm[which.min(abs(scale(flame5cm) - 3))]
flame5cm[423]
which.min(abs(scale(flame5cm) - 3))

# 최종 값
round(flame5cm[which.min(abs(scale(flame5cm) - 3))] - mean(flame5cm), digits = 0)

?(which.min)
