#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "ui/UIVideoPlayer.h"

#include <string>

class HelloWorld : public cocos2d::Layer
{
	//Параметры плеера
	const char* apiUrl = "https://puzzle-english.com/api/test.php";
	const char* videoName = "video.mp4";
	const char* jsonKey = "url";
	const char* buttonText = "Play video";
	
	//Сообщения об ошибке
	const char* boxTitle = "Error";
	const char* apiErr = "Incorrect response from the server.";
	const char* netErr = "HTTP request failed. Check Internet connection.";
	const char* fileErr = "Couldn't create video file. File may be in use or protected.";
	
	//Размер видео в процентах от высоты
	const float videoHeight = 0.8;

	//Обертка для нативных плееров Android и iOS
	cocos2d::experimental::ui::VideoPlayer* videoPlayer;
	std::string fileName;
	bool waitingForResponse;
	
	std::string getUrlFromJson(std::string json);
	void sendApiRequest();
	void sendDownloadRequest(std::string url);
	void playVideo();
public:
    static cocos2d::Scene* createScene();
	
    virtual bool init();
    
	void apiRequestCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
	void videoDownloadCallback(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
