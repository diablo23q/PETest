#include "HelloWorldScene.h"
#include "external/json/rapidjson.h"
#include "external/json/document.h"

USING_NS_CC;
using namespace network;
using namespace std;

Scene* HelloWorld::createScene() {
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init() {
    if ( !Layer::init() ) {
        return false;
    }
	
	waitingForResponse = false;
	//Получаем полное имя файла для сохранения на устройстве
    fileName = FileUtils::getInstance()->getWritablePath() + videoName;
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	//Создаём и размещаем плеер
	videoPlayer = cocos2d::experimental::ui::VideoPlayer::create();
	videoPlayer->setContentSize(visibleSize);
	videoPlayer->setPosition(Vec2( visibleSize.width / 2, visibleSize.height * videoHeight / 2 ));
	videoPlayer->setScale(videoHeight);
	videoPlayer->setKeepAspectRatioEnabled(true);
	this->addChild(videoPlayer);

	auto menu = Menu::createWithItem(MenuItemLabel::create(Label::createWithSystemFont(buttonText, "arial", 24),
		[this](Ref *sender) {
			//Если видео не играется и не скачивается в данный момент
			if(!videoPlayer->isPlaying() && !waitingForResponse) {
				//то, в зависимости от того, скачан ли файл
				if(!FileUtils::getInstance()->isFileExist(fileName)) {
					//либо начинаем скачивание
					sendApiRequest();
				} else {
					//либо играем видео
					playVideo();
				}
			}
		}
	));
	//Размещаем кнопку на оставшемся от видео (1 - videoHeight) месте
	menu->setPosition(Vec2( visibleSize.width / 2, visibleSize.height * (videoHeight + 1) / 2 )); // h + (1 - h) / 2
	this->addChild(menu);	
    
    return true;
}

//Запрос к API
void HelloWorld::sendApiRequest() {
	HttpRequest* request = new HttpRequest();
	request->setUrl(apiUrl);
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(HelloWorld::apiRequestCallback, this));
	HttpClient::getInstance()->send(request);
	waitingForResponse = true;
	request->release();
}

//Запрос к URL видео
void HelloWorld::sendDownloadRequest(string url) {
	HttpRequest* request = new HttpRequest();
	request->setUrl(url.c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(HelloWorld::videoDownloadCallback, this));
	HttpClient::getInstance()->send(request);
	waitingForResponse = true;
	request->release();
}

//Получение URL из ответа сервера
string HelloWorld::getUrlFromJson(string json) {
	string ret;
	rapidjson::Document d;
	
	d.Parse<0>(json.c_str());
	if (!d.HasParseError() && d.IsObject() && d.HasMember(jsonKey)) {
		ret = d[jsonKey].GetString();
	} else {
		MessageBox(apiErr, boxTitle);
	}
	return ret;
}

//Старт проигрывания видео
void HelloWorld::playVideo() {
	videoPlayer->setFileName(fileName);
	videoPlayer->play();
}

//Ответ на запрос к API
void HelloWorld::apiRequestCallback(HttpClient *sender, HttpResponse *response) {
	waitingForResponse = false;
    if (response && response->isSucceed()) {
        vector<char> *data = response->getResponseData();
        string json(&(data->front()), data->size());
		string url = getUrlFromJson(json);//Получаем URL видео
		if(url.length() > 0) {
			sendDownloadRequest(url);
		}
    } else {
       MessageBox(netErr, boxTitle);
    }
}

//Ответ на запрос к URL видео
void HelloWorld::videoDownloadCallback(HttpClient *sender, HttpResponse *response) {
	waitingForResponse = false;
    if (response && response->isSucceed()) {
		vector<char> *responseData = response->getResponseData();
		
		//Сохраняем видео в файл на устройстве
		FILE *fp = fopen(fileName.c_str(), "wb");
		if (!fp) {
			MessageBox(fileErr, boxTitle);
			return;
		}
		fwrite(responseData->data(), 1, responseData->size(), fp);
		fclose(fp); 
		
		//Проигрываем
		playVideo();
    } else {
       MessageBox(netErr, boxTitle);
    }
}
