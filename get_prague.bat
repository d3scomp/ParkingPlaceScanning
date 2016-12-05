'C:\Program Files (x86)\Python2.7\python.exe' 'C:\Program Files (x86)\Sumo\tools\osmGet.py' --bbox 50.0809,14.3864,50.0941,14.4187 --prefix prague

'C:\Program Files (x86)\Python2.7\python.exe' 'C:\Program Files (x86)\Sumo\tools\osmBuild.py' --prefix prague --osm-file prague_bbox.osm.xml  --vehicle-classes road --typemap 'C:\Program Files (x86)\Sumo\data\typemap\osmPolyconvert.typ.xml'

echo "Edit generated prague.poly.xml to declare everything important as building"
