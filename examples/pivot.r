df = read.csv("/tmp/neoleo.csv", header = FALSE)
#df1 <- aggregate(df, by = df[1], FUN=sum)
df1 <- aggregate(df$V2, by=list(df$V1), FUN=mean)	
print(df1)


