echo "COMPILING..."
if (cd .. && make) ; then 
    echo "RUNNING...";
    time ../snowlang main.sno
else 
    echo "COMPILATION FAILED..."; 
fi