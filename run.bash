echo "COMPILING..."
if make ; then 
    echo "RUNNING...";
    time ./snowlang $1
else 
    echo "COMPILATION FAILED..."; 
fi