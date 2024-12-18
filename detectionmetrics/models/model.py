from abc import ABC, abstractmethod
import os
from typing import Optional

import numpy as np
import pandas as pd
from PIL import Image


from detectionmetrics.datasets import dataset as dm_dataset
import detectionmetrics.utils.conversion as uc
import detectionmetrics.utils.io as uio


class SegmentationModel(ABC):
    """Parent segmentation model class

    :param ontology_fname: JSON file containing model output ontology
    :type ontology_fname: str
    :param model_cfg: JSON file containing model configuration
    :type model_cfg: str
    """

    def __init__(self, ontology_fname: str, model_cfg: str):
        # Check that provided paths exist
        assert os.path.isfile(ontology_fname), "Ontology file not found"
        assert os.path.isfile(model_cfg), "Model configuration not found"

        # Read ontology and model configuration
        self.ontology = uio.read_json(ontology_fname)
        self.model_cfg = uio.read_json(model_cfg)
        self.n_classes = len(self.ontology)

        self.model = None

    @abstractmethod
    def inference(self):
        """Perform inference"""
        raise NotImplementedError

    @abstractmethod
    def eval(
        self,
        dataset: dm_dataset.SegmentationDataset,
        split: str = "all",
        ontology_translation: Optional[str] = None,
    ) -> pd.DataFrame:
        """Perform evaluation for an image segmentation dataset

        :param dataset: Segmentation dataset for which the evaluation will be performed
        :type dataset: ImageSegmentationDataset
        :param split: Split to be used from the dataset, defaults to "all"
        :type split: str, optional
        :param ontology_translation: JSON file containing translation between dataset
        and model output ontologies
        :type ontology_translation: str, optional
        :return: DataFrame containing evaluation results
        :rtype: pd.DataFrame
        """
        raise NotImplementedError

    def get_lut_ontology(self, dataset_ontology: dict, ontology_translation: Optional[dict] = None):
        """Build ontology lookup table (leave empty if ontologies match)

        :param dataset_ontology: Image or LiDAR dataset ontology
        :type dataset_ontology: dict
        :param ontology_translation: Dictionary containing translation between model and
        dataset ontologies, defaults to None
        :type ontology_translation: Optional[dict], optional
        """
        lut_ontology = None
        if dataset_ontology != self.ontology:
            if ontology_translation is not None:
                ontology_translation = uio.read_json(ontology_translation)
            lut_ontology = uc.get_ontology_conversion_lut(
                dataset_ontology,
                self.ontology,
                ontology_translation,
                self.model_cfg.get("ignored_classes", []),
            )
        return lut_ontology


class ImageSegmentationModel(SegmentationModel):
    """Parent image segmentation model class

    :param ontology_fname: JSON file containing model output ontology
    :type ontology_fname: str
    :param model_cfg: JSON file containing model configuration (e.g. image size or
    normalization parameters)
    :type model_cfg: str
    """

    def __init__(self, ontology_fname: str, model_cfg: str):
        super().__init__(ontology_fname, model_cfg)

    @abstractmethod
    def inference(self, image: Image.Image) -> Image.Image:
        """Perform inference for a single image

        :param image: PIL image.
        :type image: Image.Image
        :return: Segmenation result as PIL image
        :rtype: Image.Image
        """
        raise NotImplementedError

    @abstractmethod
    def eval(
        self,
        dataset: dm_dataset.ImageSegmentationDataset,
        split: str = "all",
        ontology_translation: Optional[str] = None,
    ) -> pd.DataFrame:
        """Perform evaluation for an image segmentation dataset

        :param dataset: Image segmentation dataset for which the evaluation will
        be performed
        :type dataset: ImageSegmentationDataset
        :param split: Split to be used from the dataset, defaults to "all"
        :type split: str, optional
        :param ontology_translation: JSON file containing translation between dataset
        and model output ontologies
        :type ontology_translation: str, optional
        :return: DataFrame containing evaluation results
        :rtype: pd.DataFrame
        """
        raise NotImplementedError


class LiDARSegmentationModel(SegmentationModel):
    """Parent LiDAR segmentation model class

    :param ontology_fname: JSON file containing model output ontology
    :type ontology_fname: str
    :param model_cfg: JSON file containing model configuration (e.g. sampling method,
    input format, etc.)
    :type model_cfg: str
    """

    def __init__(self, ontology_fname: str, model_cfg: str):
        super().__init__(ontology_fname, model_cfg)

    @abstractmethod
    def inference(self, points: np.ndarray) -> np.ndarray:
        """Perform inference for a single image

        :param image: Point cloud xyz array
        :type image: np.ndarray
        :return: Segmenation result as a point cloud with label indices
        :rtype: np.ndarray
        """
        raise NotImplementedError

    @abstractmethod
    def eval(
        self,
        dataset: dm_dataset.LiDARSegmentationDataset,
        split: str = "all",
        ontology_translation: Optional[str] = None,
    ) -> pd.DataFrame:
        """Perform evaluation for a LiDAR segmentation dataset

        :param dataset: LiDAR segmentation dataset for which the evaluation will be
        be performed
        :type dataset: LiDARSegmentationDataset
        :param split: Split to be used from the dataset, defaults to "all"
        :type split: str, optional
        :param ontology_translation: JSON file containing translation between dataset
        and model output ontologies
        :type ontology_translation: str, optional
        :return: DataFrame containing evaluation results
        :rtype: pd.DataFrame
        """
        raise NotImplementedError
