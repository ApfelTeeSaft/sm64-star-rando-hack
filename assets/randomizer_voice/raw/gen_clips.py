import win32com.client

speaker = win32com.client.Dispatch("SAPI.SpVoice")
stream = win32com.client.Dispatch("SAPI.SpFileStream")

with open("text.txt", "r", encoding="utf-8") as f:
    lines = f.readlines()

count = 0

for line in lines:
    line = line.strip()

    if not line:
        continue

    if " - " not in line:
        print(f"Skipping: {line}")
        continue

    filename, text = line.split(" - ", 1)

    print(f"[{count+1}/{len(lines)}] {filename}")

    stream.Open(filename, 3)
    speaker.AudioOutputStream = stream
    speaker.Speak(text)
    stream.Close()

    count += 1

print(f"Generated {count} files.")