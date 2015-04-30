#include "HelloWorldScene.h"
#include "ui/UIVideoPlayer.h"
#include "external/json/rapidjson.h"
#include "external/json/document.h"

#include <vector>
#include <string>

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
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto menu = Menu::createWithItem(MenuItemLabel::create(Label::createWithSystemFont("Play video", "Arial", 24),
		[this](Ref *sender) {
			HttpRequest* request = new HttpRequest();
			request->setUrl("https://puzzle-english.com/api/test.php");
			request->setRequestType(HttpRequest::Type::GET);
			request->setResponseCallback(CC_CALLBACK_2(HelloWorld::apiRequestCallback, this));
			HttpClient::getInstance()->send(request);
			request->release();
		}
	));
	menu->setPosition(visibleSize / 2);
	this->addChild(menu);
    
    return true;
}


void HelloWorld::apiRequestCallback(HttpClient *sender, HttpResponse *response) {
    if (response && response->getResponseCode() == 200 && response->getResponseData()) {
        vector<char> *data = response->getResponseData();
        string ret(&(data->front()), data->size());
        CCLOG("%s", ("Response message: " + ret).c_str());

		rapidjson::Document d;
		d.Parse<0>(ret.c_str());
		if (d.HasParseError()) {
			CCLOG("GetParseError %s\n",d.GetParseError());
		}

		string url;
		if (d.IsObject() && d.HasMember("url")) {
			url = d["url"].GetString();
		}

		HttpRequest* request = new HttpRequest();
		request->setUrl(url.c_str());
		request->setRequestType(HttpRequest::Type::GET);
		request->setResponseCallback(CC_CALLBACK_2(HelloWorld::videoDownloadCallback, this));
		HttpClient::getInstance()->send(request);
		request->release();
    } else {
       
    }
}

void HelloWorld::videoDownloadCallback(HttpClient *sender, HttpResponse *response) {
    if (response && response->getResponseCode() == 200 && response->getResponseData()) {
		vector<char> *responseData = response->getResponseData();
		string filename = FileUtils::getInstance()->getWritablePath() + "thevideo.mp4";
		FILE *fp = fopen(filename.c_str(), "wb");
		if (! fp) {
			CCLOG("can not create file");
			return;
		}
		fwrite(responseData->data(), 1, responseData->size(), fp);
		fclose(fp);

		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto videoPlayer = cocos2d::experimental::ui::VideoPlayer::create();
		videoPlayer->setContentSize(visibleSize);
		videoPlayer->setPosition(Vec2(visibleSize.width / 4, (visibleSize.height / 4 * 3)));
		videoPlayer->setScale(0.5);
		videoPlayer->setKeepAspectRatioEnabled(true);
		this->addChild(videoPlayer);
		videoPlayer->setFileName(filename);
		if(FileUtils::getInstance()->isFileExist(filename)) videoPlayer->play();
    } else {
       
    }
}
