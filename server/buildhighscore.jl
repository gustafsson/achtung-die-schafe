using DataFrames
using Base.Dates;

fn = length(ARGS) >= 1 ? ARGS[1] : "logfile.txt"
tostdout = length(ARGS) >= 1 ? ARGS[end] == "-" : false

timestamp = nothing;
d = map(readlines(fn)) do l
  try
    global timestamp
    timestamp = DateTime(l, "y-m-dTH:M:S");
    return DataFrame();
  catch e
    # ignore
  end

  m = match(r"([0-9]*)\. (.*) \(id=[0-9]*, ([0-9:]*), ([0-9:]*)\) (\(observer\) )?([0-9\.]*)", l);

  if m == nothing
    println("Failed to parse row (ignoring): ", l);
    return DataFrame();
  else
    DataFrame( score = parse(Float64, m.captures[1])
             , nick = m.captures[2]
             , playtime = Float64(Millisecond(DateTime(m.captures[3],"H:M:S")-DateTime()))/1000
             , alivetime = Float64(Millisecond(DateTime(m.captures[4],"H:M:S")-DateTime()))/1000
             , wasactive = m.captures[5] == nothing
             , ip = m.captures[6]
             , timestamp = timestamp
             )
  end
end;
d = vcat(DataFrame[d...]);

s = by(d,:nick) do b
  DataFrame( score = sum(b[:score])
            , playtime = sum(b[:playtime])
            , alivetime = sum(b[:alivetime])
            , wasactive = sum(b[:wasactive])/nrow(b)
            , ip = join(unique(b[:ip]),", ")
            , firstseen = minimum(b[:timestamp]) - Millisecond(b[1,:alivetime]*1000)
            , lastseen = maximum(b[:timestamp])
            )
end;
s[:score_per_alive_minute] = s[:score] ./ s[:alivetime];
s[:alive_fraction] = s[:alivetime] ./ s[:playtime];
sort!(s,cols=:score,rev=true);

outfn = tostdout ? tempname() * ".csv" : "highscore.csv"
writetable(outfn, s)

if tostdout
  println(readall(outfn))
  rm(outfn)
end
