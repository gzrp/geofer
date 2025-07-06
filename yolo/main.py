import time
from typing import List
from ultralytics import YOLO

import uvicorn
from fastapi import FastAPI, File, Form, UploadFile, HTTPException
from pydantic import BaseModel
from PIL import Image
from io import BytesIO

app = FastAPI(title="Image Analyze Service", version="1.0")

yolo_model = YOLO('best.pt')


# 图像坐标系示意：
#
#    y
#    ↓
#    0 ┌────────────────────────────→ x
#      │
#      │   (x1, y1)         (x2, y1)
#      │     ┌──────────────┐
#      │     │              │
#      │     │   目标物体    │
#      │     │              │
#      │     └──────────────┘
#      │   (x1, y2)         (x2, y2)
#      │
#      ↓

class ObjectBox(BaseModel):
    label: str
    confidence: float
    bbox: List[float]           # x1, y1, x2, y2        左上角         右下角

class AnalyzeResult(BaseModel):
    objects: List[ObjectBox]


def dummy_object_detection() -> List[ObjectBox]:
    return [
        ObjectBox(label="银行", confidence=0.95, bbox=[30, 150, 130, 210]),
        ObjectBox(label="好客连锁", confidence=0.95, bbox=[135, 200, 210, 235]),
        ObjectBox(label="落日", confidence=0.95, bbox=[310, 220, 400, 270]),
        ObjectBox(label="路灯", confidence=0.90, bbox={160, 100, 170, 220}),
        ObjectBox(label="摩托车", confidence=0.93, bbox={580, 410, 640, 470}),
        ObjectBox(label="骑车人", confidence=0.92, bbox={570, 370, 650, 450}),
        ObjectBox(label="交通护栏", confidence=0.88, bbox={100, 320, 330, 360}),
    ]


def yolo_object_detection(image : Image.Image) -> List[ObjectBox]:

    results = yolo_model(image, show=False, save=True)
    boxes = results[0].boxes
    cls_names =  results[0].names

    detected_objects = []

    for i in range(len(boxes)):
        box = boxes[i]
        x1, y1, x2, y2 = box.xyxy[0].tolist()
        confidence = box.conf[0].item()
        class_id = int(box.cls[0].item())
        label = cls_names[class_id]
        detected_objects.append(ObjectBox(label=label, confidence=confidence, bbox=[x1, y1, x2, y2]))

    return detected_objects


@app.post("/api/v1/image_analyze")
async def analyze_image(description: str = Form(...), file: UploadFile = File(...)):
    start = time.time()
    content = await file.read()
    file_size  = len(content)

    # 使用 PIL 获取图像尺寸
    image = Image.open(BytesIO(content))
    width, height = image.size

    print("width: {}, height: {}, file_size: {}".format(width, height, file_size))
    print("description: {}".format(description))

    # objects = dummy_object_detection()
    objects = yolo_object_detection(image)
    result = AnalyzeResult(
        objects=objects
    )
    print(result)
    end = time.time()
    print("spend time: {}", end - start)
    return result


if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=8080, reload=True)