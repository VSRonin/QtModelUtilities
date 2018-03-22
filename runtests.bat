for %%a in (*.exe) do (
%%a -o %%a.xml,xunitxml 
curl -i -X POST -H "Content-Type: multipart/form-data" -F "data=@%%a.xml" https://ci.appveyor.com/api/testresults/xunit/%APPVEYOR_JOB_ID%
)