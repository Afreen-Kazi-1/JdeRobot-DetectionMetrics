import argparse

from PIL import Image

from detectionmetrics.datasets import Rellis3DImageSegmentationDataset
from detectionmetrics.models import TorchImageSegmentationModel
import detectionmetrics.utils.conversion as uc
from torchvision.models.segmentation import deeplabv3_resnet50


def parse_args() -> argparse.Namespace:
    """Parse user input arguments

    :return: parsed arguments
    :rtype: argparse.Namespace
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--model", type=str, default="deeplabv3_resnet50", help="Model name"
    )
    parser.add_argument(
        "--model_ontology",
        type=str,
        required=True,
        help="JSON file containing model output ontology",
    )
    parser.add_argument(
        "--model_cfg",
        type=str,
        required=True,
        help="JSON file with model configuration (norm. parameters, image size, etc.)",
    )
    parser.add_argument(
        "--image", type=str, required=False, help="Image that will be segmented"
    )
    parser.add_argument(
        "--dataset_dir",
        type=str,
        required=True,
        help="Directory where dataset images and labels are stored",
    )
    parser.add_argument(
        "--split_dir",
        type=str,
        required=True,
        help="Directory where .lst files defining the dataset split are stored",
    )
    parser.add_argument(
        "--dataset_ontology",
        type=str,
        required=False,
        help="JSON file containing dataset ontology",
    )
    parser.add_argument(
        "--out_fname",
        type=str,
        required=True,
        help="CSV file where the evaluation results will be stored",
    )
    return parser.parse_args()


def main():
    """Main function"""
    args = parse_args()

    dataset = Rellis3DImageSegmentationDataset(
        args.dataset_dir, args.split_dir, args.dataset_ontology
    )

    if args.model == "deeplabv3_resnet50":
        torch_model = deeplabv3_resnet50(num_classes=len(dataset.ontology))
    else:
        raise ValueError("Model not recognized")

    model = TorchImageSegmentationModel(
        torch_model, args.model_cfg, args.model_ontology
    )

    if args.image is not None:
        image = Image.open(args.image).convert("RGB")
        result = model.inference(image)
        result = uc.label_to_rgb(result, model.ontology)
        result.show()

    results = model.eval(dataset)
    results.to_csv(args.out_fname)


if __name__ == "__main__":
    main()
