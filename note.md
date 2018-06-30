* 2018, June 25

여러가지 방법들이 있지만. 다 잘 안되고..
master branch에 현재 막 추가되고 있는 기능 중에. (사실, anchor node라는 것도 아직 개발중인거 같다.)
root라는 개념이 있는데, 이걸 사용해보는걸 해보자.

먼저 painlessmesh를 최신으로 당기고.
setRoot
setContainsRoot(true);

뭐.. 좋아진건지 어떤지.. 일단 root 때문에.. root가 나타나면 일단 root랑 연결한 녀석만 빼고.. 다들 아노미 상태..
root를 중심으로 다시 빌드업하는 것 같은건 좋은데..

그런담에.. 뭔가 메세지가 전파되는 속도가.. 실시간성이 뭔가 안지켜지는 것 같은 감이 있었다.

(arduinojson 은 beta 버젼을 쓰면 안됨.)
(painlessmesh init()에서 바뀐것들이 있어서 맞춰줌.)
(arduino-esp32를 git pull 해야했음.)

---

다음은 깃발들. 과.. 동그라미들. 을 따로 커멘드 할수 있어야 한다는 것인데..
노드 이름을 부여하고.. 하는 등등..

---

이름 같은거 하지 말고. 그냥 메세지 받고.. 커멘드에 첫번째 글자 같은걸로 확인하면 되지 않을까 싶다.

---

anchor node를 어떻게 만드는가하는 질문이 있었는데.
최근 merge된 commit중에서.. root노드를 정하고. root node에 우선/최종 접속하는 노드를 setContainsRoot라는 함수로 정하는 기능이 추가되고.
anchor node에 대한 commit은 빠진걸 보았는데..
생각해보니.
anchor node가 뭐냐.
"Introduce an anchor (master) node, causing the sub mesh that contains such an anchor node never to try to reconnect. In the above example the bridge would be an anchor node and as a result B would never decide to disconnect from this node. Similarly, when C connects to B it will also be anchored in place and wouldn't try to restructure the network. The existence of the anchor node will be (optionally) included in the node sync message."
이걸 잘 생각해보면..
결국 root node가 anchor 노드이고.. root를 인식하는 node가 anchor node가 된다는 식으로 이해할 수도 있다는 말이된다.
setContainsRoot를. 사용하는 노드가 있고. 사용하지 않는 노드가 있다면.. freenode는 정말 freenode가 되는 거고.. 그렇게 해서. 유연함/속도와 안정성/수렴성을 trade off 할 수 있다는 이야기가 아닐까 싶네.

---

현재는 노드가 전부 free node.
준비한 코드는 전부 anchor node.
이중에 일부분만 전략적으로.. anchor node (setContainsRoot) 화 하는 것도 방법이다.

---

테스트 1

all free node with updated s/w + a root node Master




* 2018, June 23

* txpower
* maxconn
* sta only or ap only..
* having graphical monitoring system.. (rpi or a webpage)
* 'anchor' node..
* use esp32 (maxconn == 10)

뭔가 가장 확실하고 간단한 방법으로 해결했으면 좋겠다.
일단 monitoring system을 만드는 것은 좀.. 자신이 없나 싶기도 하고.... 문제를 인식하고 debug하는데는 도움이 되겠지만, 직접적으로 해결하는 effort는 되지 않는다.
anchor node란 어떻게 만들어질까.
stationManual을 이용해서.. 다른 mesh에 고정식으로 접속하게 할 수 있다.
mesh1/2/3/4가 있고. 대표자들이 있고. 대표자들이 중앙시스템에 접속하게 한다. 다만.. 대표자 그룹이 4개까지만 가능. (maxconn == 4)
예를들면.. 깃발들이 총 4개라면 깃발들만 대표자를 한다거나.
여튼 그런식으로.
2 layer.
이렇게 되면 mesh가 4개가 되고. 각 대표자들이 mainmesh에 접속하게 된다.
각 대표자들은 3개까지 데리고 올수있다.
즉.. 이렇게 되면 12개까지만 가능.
즉.. 동그라미들 12개는 이렇게 관리 가능하긴하다.
하지만 깃발 6개.
깃발이 6개니까.. 3명씩 관리하면 18개가 된다.
그럼 딱 되네.

근데 그랬을때 hetero mesh 를 통과하는 것은.. 아직 해본적이 없다. mqtt도 잘 안되었는데.. 같은 문제 일까.
txpower maxconn 은.. 그냥 참고할만한 정도 밖에 안된다.
깃발 6개를 받으려면. 필수적으로.. controller는 esp32여야 한다. (maxconn == 10)
일단 가지고 있는 esp32들을 이용해서 테스트 해볼 수는 있다.

---

문화비축기지 T4. 실린더형 (두꺼운) 철재 구조물 안에 soundmesh 고정형 설치 mesh nodes == 18

문제점들: 각 node들이 혼란을 겪고 있는 듯이 보인다. 중간중간 재설정하는 노드가 보인다. controller 노드도 접속을 해내지 못한다. 노드넷이 안정화되는데 상당한 시간이 걸리고, 컨트롤러 노드도 갑자기 쫒겨나게 된다거나. 많은 시간동안 연결 부재 상태이다.

painlessmesh api 문서를 다시한번 찬찬히 보았다.
init 함수, 즉 설정부분을 잘 보아야 할것 같은데. 기존의 설정에서 사용하지 않고 있는 선택 옵션들이 더 있다.

목표: 전체 노드가 하나의 매쉬로 최대한 빨리, 안정적으로 수렴할 것. 컨트롤러를 포함하여

wifi protocol 옵션 ==> 11g 기본선택
tx power 옵션 ==> 82, [0, 82] 선택 가능. 즉, 최대치.
maxconnection 옵션 ==> 4, esp8266에서는 espressif 에서도 4이고 이것이 곧 최대치라고 하고 있음.
sta_ap 등 설정옵션 ==> 기본 sta_ap..

"All nodes know the complete topology instantly" -> 컨트롤러가 들어있는지 없는지 확인하고서. 없다면 네트웤을 다시 찾게 할수도 있을지 모른다.

기본적으로 controller 노드가 esp8266 한대이니까.. 접속 가능 수가 4밖에 안된다. 친구를 한명데리고 접속을 시작한다는 방식이 그래서 유리하지만. 역시 둘이서 접속가능수는 8밖에 안된다.
"There is a limit of 4 station nodes per AP. This limit is fixed by ESP8266 SDK"
"Every node knows the complete network topology." -> connection list를 뽑아보고 모니터링하는 controller이면 좋을 것이다.
mesh design이 필요함.

"Network loops are actively avoided" -> 이것때문에 계속 reconfigure가 발생하는 것 같기도 하다.

https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation

i think this explains very well what i am now facing.
and in the end it proposes having 'anchor' nodes in the net, which will never try reconfigure, constantly trying to setup connection to the designated master, keeping alive the possibility to 'full mesh'

well, i don't know how to do 'anchor' mesh, yet. but basically same approach that's in my mind.

"Have two devices communicate via the serial connection." so this also means.. having hetero-type node (e.g. rpi etc.)
https://gitlab.com/painlessMesh/painlessMeshListener -> a D program ready for rpi.. well.. D looks like normal C code.. but i don't know D yet.

and also MQTT: https://gitlab.com/painlessMesh/painlessMesh/blob/master/examples/mqttBridge/mqttBridge.ino

"Another possible solution is to have your bridge node run in AP only mode..."
so we can let them be only AP.. don't self-configure but i will connect to all one-by-one

---

so now.. i feel that i collected all (possible) info. available to solve the issue.

---

* 2018, May 3

--- (노드 통신 지연 시간에 대한 고려 여부)
전체적으로.. command queue 기능이 필요할까?
하지만.. 지휘자가 지휘를 전달하는데 시간이 1초씩 걸린다고 하면..
그건.. 지휘/연주가 될 수 있을까?
실시간이 안되면, 되는 상황으로 만드는 것이 맞다.

--- (사운드 레이어 가능성 여부)
동시에 여러 곡 연주의 문제
틴지는 컴퓨터 처럼 메모리가 그렇게 크지 않기 때문에..sd 카드 읽기 속도가 메모리 처럼 혹은 충분히 빨라야 한다.
teensy 3.x에서 추가적인 sd read의 성능 개선이 있고, 3.5/6에서 USE_SDIO 등이 사용 가능하다고 한다.
따라서.. teensy에서 동시 플레이가 이미 가능할 수 있다.
테스트가 먼저. 지금은 간단하게. 다만, 동시플레이가 불가능하지 않다는 점을 기억.

---
일단 가장 심플하게는.. 한가지 음원을 한쪽 채널에서 플레이 하고.
새로운 음원의 큐가 오면.. 그것으로 fade out-->in 하거나. 즉시 switch 하면 된다.

---
start - restart if playing
stop - stops if play ends
same for 2 ch. for now
no fading for now
no info for now, only # of nodes

---
관심있는 추가 기능

* 게이트웨이-웹서버ip
* 노드들의 배터리전압
* 노드 clock
* 채널 1/2 선택
* 노드 플레이 pause
* 노드 이벤트 n초후

---
