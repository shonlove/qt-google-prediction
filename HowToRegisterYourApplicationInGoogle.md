# Introduction #

The **qt-google-prediction** example uses Google Prediction API. To use API you need to register your own application on Google.


# Details #

You need to login to Google, so first you need to create simple Google account. Then you can visit the page

https://code.google.com/apis/console

there you can create your application. You need to check access to Google Prediction API and Google Cloud Storage. Please, note, that you have to turn on billing for your application, as Prediction API has a monthly fee $10.



Then  you can see credentials of your application. You need to copy and paste **Client\_ID**, **Client\_Secret** and **Redirect\_URI** to the file **oauth2.cpp**.
```
OAuth2::OAuth2(QWidget * p)
    : QObject(p)
{
    m_strEndPoint = "https://accounts.google.com/o/oauth2/auth";
    m_strScope = "https://www.googleapis.com/auth/devstorage.read_only"
            "+https://www.googleapis.com/auth/prediction";

    m_strClientID = "YOUR_CLIENT_ID_HERE";
    m_strClientSecret = "YOUR_CLIENT_SECRET_HERE";
    m_strRedirectURI = "YOUR_REDIRECT_URI_HERE";
    m_strSimpleAPIKey = "YOUR_SIMPLE_API_KEY_HERE";


```

After that you can compile and run **qt-google-prediction**.