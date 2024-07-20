import googleapiclient.discovery
from urllib.parse import parse_qs, urlparse
from dotenv import load_dotenv
import webbrowser
import time
import pytube
import serial
import os

load_dotenv()
DEVELOPER_KEY = os.getenv('DEVELOPER_KEY')
ARDUINO_PORT = os.getenv('ARDUINO_PORT')

arduinoData = serial.Serial(ARDUINO_PORT, 9600)
time.sleep(1)

#extract playlist id from url
url = 'https://www.youtube.com/playlist?list=PLjJmNf2OBjErWYejxT3KPNgX7cUNV3B5D'
query = parse_qs(urlparse(url).query, keep_blank_values=True)
playlist_id = query["list"][0]

print(f'get all playlist items links from {playlist_id}')
youtube = googleapiclient.discovery.build("youtube", "v3", developerKey = DEVELOPER_KEY)

request = youtube.playlistItems().list(
    part = "snippet",
    playlistId = playlist_id,
    maxResults = 50
)
response = request.execute()

playlist_items = []
while request is not None:
    response = request.execute()
    playlist_items += response["items"]
    request = youtube.playlistItems().list_next(request, response)

print(f"total: {len(playlist_items)}")
urls = []
for t in playlist_items:
    video_id = t["snippet"]["resourceId"]["videoId"]
    url = f'https://www.youtube.com/watch?v={video_id}&list={playlist_id}&t=0s'
    urls.append(url)

player_idx = 0
while True:
    while (arduinoData.inWaiting()==0):
        pass
    play = arduinoData.readline().decode('utf-8').strip()
    if (play == "1"):
        print(urls[player_idx])
        yt = pytube.YouTube(urls[player_idx])
        webbrowser.open(urls[player_idx])
        player_idx += 1
        time.sleep(1)
        # send one back to arduion signalling youtube video has started
        arduinoData.write("1".encode())
