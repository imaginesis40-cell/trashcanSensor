#************** test*******************#
getwd()
virtualData <- scan("data/virtualData.csv.txt")
print(virtualData)

# 5cm 거리일 때 불꽃감지센서 데이터
flame5cm <- scan("C:data/dataof5cm.txt", sep = ",", what = list(NULL, 0))
flame5cm <- (flame5cm)[[2]]

matrix(flame5cm, nrow = length(flame5cm), ncol = 1,
       dimnames = list(1:599, c("5cm")));

# 10cm 거리일 때 불꽃감지센서 데이터

flame10cm <- scan("C:data/dataof10cm.txt", sep = ",", what = list(NULL, 0))
flame10cm <- (flame10cm)[[2]]
matrix(flame10cm, nrow = length(flame5cm), ncol = 1,
       dimnames = list(NULL, c("10cm")));

# 데이터 열로 묶기
scaledFlameData <- cbind(flame5cm, flame10cm)

# 데이터 scale. z-score = (값 - 평균)/표준편차.
# 1.0 (1시그마) ->  상위 15%
# 2.0 (2시그마) ->  상위 2.3%
scaledFlameData <- apply(scaledFlameData, 2, scale)
scaledFlameData
scaled5cm <-  sort(scaledFlameData[, 1])
scaled10cm <-  sort(scaledFlameData[, 2])

depth <- scan("C:data/dataofdepth.txt", sep = ",", what = list(NULL, 0))
depth
depth <- (depth)[[2]]
depth <-  matrix(depth, nrow = length(depth), ncol = 1,
                 dimnames = list(1:100, c("depth")));
scaleddepth <- apply(depth, 2, scale); scaleddepth

# 종합 산점도

idx <- seq(1, length(flame5cm), 10)
framedFlame <- data.frame(flame5cm[idx], flame10cm[idx]);
colnames(framedFlame) <- c("5cm거리", "10cm거리")
rownames(framedFlame) <-1:length(flame5cm[idx])
framedFlame
plotFlame <- stack(framedFlame)
plotFlame

ggplot(plotFlame, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 17, size = 1) + labs(x = "거리", y = "아날로그 값") + scale_color_gradient(low = "green", high = "red")



# 5cm 산점도

framedFlame <- data.frame(flame5cm[idx]);
colnames(framedFlame) <- c("5cm거리")
rownames(framedFlame) <-1:length(flame5cm[idx])
framedFlame
plotFlame <- stack(framedFlame)
plotFlame

ggplot(plotFlame, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 16, size = 5) + labs(x = "거리", y = "아날로그 값") + scale_color_gradientn(colors = c("#202060", "#00FF00", "#FF0000"))

abs((range(flame5cm))[1] - (range(flame5cm))[2])
# 거리가 가까울수록 오차가 커지는 경향이 있음 240 ~ 70
# 10cm 산점도

framedFlame <- data.frame(flame10cm[idx]);
colnames(framedFlame) <- c("10cm거리")
rownames(framedFlame) <-1:length(flame5cm[idx])
framedFlame
plotFlame <- stack(framedFlame)
plotFlame

ggplot(plotFlame, mapping = aes(x = ind, y = values)) + geom_point(aes(color = values), shape = 16, size = 5) + labs(x = "거리", y = "아날로그 값") + scale_color_gradientn(colors = c("#202060", "#FF0000"))

abs((range(flame10cm))[1] - (range(flame10cm))[2])
