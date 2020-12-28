for dir in */; do
for file in ../../gis/$dir/*.h; do python3 createWrapper.py $file; done;
done
for file in ../../gis/*.h; do python3 createWrapper.py $file; done;
