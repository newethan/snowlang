echo "COMPILING..."
if (cd .. && make) ; then 
    echo "RUNNING...";
    time ../snowlang sncomputer.sno
else 
    echo "COMPILATION FAILED..."; 
fi