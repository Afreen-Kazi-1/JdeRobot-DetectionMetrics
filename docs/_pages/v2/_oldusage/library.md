---
layout: home
title: Library
# permalink: /v2/usage/library/

sidebar:
  nav: "main_v2"
---

DetectionMetrics currently provides a CLI with two commands, `dm_evaluate` and `dm_batch`. Thanks to the configuration in the `pyproject.toml` file, we can simply run `poetry install` from the root directory and use them without explicitly invoking the Python files.

### `dm_evaluate`
Run a single evaluation job given a model and dataset configurations.

Example:
```shell
dm_evaluate segmentation image --model_format torch --model /path/to/model.pt --model_ontology /path/to/ontology.json --model_cfg /path/to/cfg.json --dataset_format rellis3d --dataset_dir /path/to/dataset  --dataset_ontology /path/to/ontology.json --out_fname /path/to/results.csv
```

Docs:
```shell
Usage: dm_evaluate [OPTIONS] {segmentation} {image|lidar}

  Evaluate model on dataset

Options:
  --model_format [torch|tensorflow]
                                  Trained model format  [default: torch]
  --model PATH                    Trained model filename (TorchScript) or
                                  directory (TensorFlow SavedModel)
                                  [required]
  --model_ontology FILE           JSON file containing model output ontology
                                  [required]
  --model_cfg FILE                JSON file with model configuration (norm.
                                  parameters, image size, etc.)  [required]
  --dataset_format [gaia|rellis3d|goose|generic]
                                  Dataset format  [default: gaia]
  --dataset_fname FILE            Parquet dataset file
  --dataset_dir DIRECTORY         Dataset directory (used for 'Rellis3D'
                                  format)
  --train_dataset_dir DIRECTORY   Train dataset directory (used for 'GOOSE'
                                  and 'Generic' formats)
  --val_dataset_dir DIRECTORY     Validation dataset directory (used for
                                  'GOOSE' and 'Generic' formats)
  --test_dataset_dir DIRECTORY    Test dataset directory (used for 'GOOSE' and
                                  'Generic' formats)
  --data_suffix TEXT              Data suffix to be used to filter data (used
                                  for 'Generic' format)
  --label_suffix TEXT             Label suffix to be used to filter labels
                                  (used for 'Generic' format)
  --dataset_ontology FILE         JSON file containing dataset ontology (used
                                  for 'Generic' format)
  --split [train|val|test]        Name of the split to be evaluated  [default:
                                  test]
  --ontology_translation FILE     JSON file containing translation between
                                  dataset and model classes
  --out_fname PATH                CSV file where the evaluation results will
                                  be stored  [required]
  --help                          Show this message and exit.
```

### `dm_batch`
Execute requested jobs sequentially. It must be configured by means of a YAML file.

Example:
```shell
dm_batch evaluate /path/to/batch_config.yaml
```

Docs:
```shell
Usage: dm_batch [OPTIONS] {evaluate} JOBS_CFG

  Perform detection metrics jobs in batch mode

Options:
  --help  Show this message and exit.
```

YAML file example:
```yaml
task: segmentation  # Task to perform (e.g., segmentation)
input_type: image  # Input type (e.g., image or lidar)
id: batch_id  # Batch identifier

# All models and datasets defined will be evaluated all-vs-all

model:
  - id: "model_id"  # Model identifier, if path is a pattern, model basename will be added as suffix
    path: "/path/to/model.pth"  # Path to the trained model file. It can be a pattern to match multiple model files (which will be evaluated independently)
    path_is_pattern: false  # Whether the path is a pattern or not
    format: torch  # Model format (e.g., torch, tensorflow)
    ontology: "/path/to/model_ontology.json"  # Path to the model output ontology JSON
    cfg: "/path/to/model_config.json"  # Path to the model configuration JSON
  - id: "another_model_id"
    # ...
  - id: "yet_another_model_id"
    # ...

dataset:
  - id: "dataset_id"  # Dataset identifier
    format: gaia  # Dataset format (e.g., gaia, rellis3d, goose, generic)
    fname: "/path/to/dataset.parquet"  # (For Gaia) Path to the dataset Parquet file
    dir: "/path/to/dataset_directory"  # (For Rellis3D) Path to the dataset directory
    train_dir: "/path/to/train_dataset_directory"  # (For Goose/Generic) Train directory
    val_dir: "/path/to/val_dataset_directory"  # (For Goose/Generic) Validation directory
    test_dir: "/path/to/test_dataset_directory"  # (For Goose/Generic) Test directory
    data_suffix: "_image.jpg"  # (For Generic) Data suffix
    label_suffix: "_label.png"  # (For Generic) Label suffix
    ontology: "/path/to/dataset_ontology.json"  # (For Generic) Path to dataset ontology
    split: test  # Dataset split to evaluate (e.g., train, val, test)
  - id: "another_dataset_id"
    # ...

outdir: "/path/to/output_directory"  # Path to output directory
overwrite: false  # Whether to overwrite existing output files or not
ontology_translation: "/path/to/ontology_translation.json"  # (Optional)
```