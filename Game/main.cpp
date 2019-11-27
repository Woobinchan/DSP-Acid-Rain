#include "Client.cpp"
#include <SFML/Audio.hpp>

int main() {

    char ip[20];
    cout<<"서버의 IP주소를 입력해주세요.";
    cin>>ip;
    serfd = socket(AF_INET, SOCK_STREAM, 0);
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = inet_addr(ip);
    seraddr.sin_port = htons(7777);
    if (connect(serfd, (sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }

    running = true;
    pthread_create(&readthread, NULL, Read, NULL);
    pthread_detach(readthread);

    Write(7,L"시작");

    while (wait) {
    }

    size_t level = 4; // 단어 내려오는 속도 조절
    Clock clock;
    Clock sectimer;
    RenderWindow window(VideoMode(1280, 720), L"산성비 테스트");
    list<Text>::iterator iter;

    sf::Music music;
    if (!music.openFromFile("BGM.wav"))
        return -1; // error

    music.setLoop(true);
    music.play();

    /* BG */
    Texture texture;
    Sprite sprite;

    /* Line */
    Vertex line1[2];
    Vertex line2[2];
    Vertex line3[2];

    /*HP Text*/
    Vertex HPtop[2];
    Vertex HPmiddle[2];

    /* rectangle */
    ConvexShape right;
    ConvexShape below;

    /*HP Bar*/
    ConvexShape HPshape;
    ConvexShape HPbar;

    /*HP Text*/
    Text HPtitle;
    Text HPtopvalue;
    Text HPmidvalue;

    /*Typing & Attack Text*/

    Text typing;
    Text attack;

    game.setShape(texture, sprite, line1, line2, line3, HPtop, HPmiddle, right,
                  below, HPshape, HPbar, HPtitle, HPtopvalue, HPmidvalue,
                  typing, attack);

    // for (int i = 0; i < 10; i++)
    //     game.severAtk();

    while (window.isOpen()) { // 창 열림
        Event event;

        while (window.pollEvent(event)) { //이벤트 처리
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) { // Typing 처리
                Uint32 uni = event.text.unicode;
                typeWord(event, uni, game);
            }
            //////////////////////////1초마다 단어
            ///추가///////////////////////////////
        }

        //////////////////////////화면 그리기//////////////////////////////

        window.clear();
        window.draw(sprite); // 배경화면 띄우기

        window.draw(right); // 모형 그리기
        window.draw(below);
        window.draw(HPshape); // HP 모형 그리기

        window.draw(line1, 2, sf::Lines); // 라인 그리기
        window.draw(line2, 2, sf::Lines);
        window.draw(line3, 2, sf::Lines);
        window.draw(HPtop, 2, sf::Lines); // HP 라인 그리기
        window.draw(HPmiddle, 2, sf::Lines);

        window.draw(HPtitle); // HP text 그리기
        window.draw(HPtopvalue);
        window.draw(HPmidvalue);

        window.draw(typing);
        window.draw(attack);

        /* 입력받은 단어 처리 */
        std::wstring text = game.TEXTBOX;
        text += tempcombi();
        game.setTyping(text);
        window.draw(game.getTyping());

        //////////////////////////화면 갱신///////////////////////////////
        Time elapsed = clock.restart();


        game.update(elapsed, game, level);

        /*-------- sendAtkWord(queue)에 단어가 존재하는 경우 -----------*/

        if (!game.sendAtkWord.empty()) {
            Write(2,game.sendAtkWord.front()); // 서버로 전송
            game.sendAtkWord.pop();
        }

        /*HP 갱신*/
        HPbar.setPoint(
            0, sf::Vector2f(1150, 720 - game.getPlayer().getPlayerHP() * 3));
        HPbar.setPoint(
            1, sf::Vector2f(1250, 720 - game.getPlayer().getPlayerHP() * 3));

        window.draw(HPbar);

        /*----------------------게임 종료 요건----------------------*/

        if (game.getPlayer().getPlayerHP() <=
            0) { // HP가 0 이하가 될 경우 게임종료
            game.setResultImage(texture, sprite,
                                false); // 패배 이미지 가져오기
            Write(6,L"죽음");
            while (sectimer.getElapsedTime().asMilliseconds() < 5000) {
                window.clear();
                window.draw(sprite);
                window.display();
            }

            break;
        }

        if(!game.tlist.empty())
        {
        for (iter = game.tlist.begin(); iter != game.tlist.end(); iter++) {

            window.draw(*iter);
        }
        }

        int ypos=50;
        if(!game.alist.empty())
        {
        for (iter = game.alist.begin(); iter != game.alist.end(); iter++) {
            iter->setPosition(1105, ypos);
            window.draw(*iter);
            ypos+=55;
        }
        }

        window.display();
        //////////////////////////////////////////////////////////////////
    } //창 닫힘

    return 0;
}
